#include <demo.hpp>

void	DemoFile::handleGameEventList(GameEventList &ge)
{
	gEvents.reserve(ge.descriptors_size());
	for (int i = 0; i < ge.descriptors_size(); i++)
	{
		const GameEventList_descriptor_t &dsc = ge.descriptors(i);
		gEvents.push_back(dsc);
	}
}

void	DemoFile::handleGameEvent(GameEvent &ge)
{
	if ((size_t)ge.eventid() >= gEvents.size())
		throw std::overflow_error("Eventid is more than gEvents");

	if (eventHooks[svc_GameEvent])
		eventHooks[svc_GameEvent](&ge);	
}

const GameEventList_descriptor_t & DemoFile::getGameEvent(size_t idx)
{
	return (gEvents[idx]);
}

void DemoFile::handleServerInfo(ServerInfo &si)
{
	info = &si;

	if (eventHooks[svc_ServerInfo])
		eventHooks[svc_ServerInfo](&si);
}

void DemoFile::handleCreateStringTable(CreateStringTable &si)
{
	parsedTables.push_back(ParsedStringTable(si, *this));

	if (eventHooks[svc_CreateStringTable])
		eventHooks[svc_CreateStringTable](&si);
}

void DemoFile::handleUpdateStringTable(UpdateStringTable &si)
{
	if ((size_t)si.table_id() >= parsedTables.size())
		return;
	const std::string &tableName = parsedTables[si.table_id()].origin.name();

	ParsedStringTable &target = parsedTables[si.table_id()];
	if (tableName == "userinfo")
		target.Update(si, *this, true);
	if (eventHooks[svc_UpdateStringTable])
		eventHooks[svc_UpdateStringTable](&si);
}

void DemoFile::handlePacketEntities(PacketEntities &e)
{
	auto	startTime = std::chrono::high_resolution_clock::now();
	(void)e;

	entities.parse(e, dataTable);
	auto	endTime = std::chrono::high_resolution_clock::now();
	auto	diffdTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	// std::clog << "[p] Time it took for packet parsing was " << diffdTime.count() << "us, len: " << e.entity_data().length() << std::endl;
	startTime = std::chrono::high_resolution_clock::now();

	std::vector<GameEntities::StagedChange *> &ent = entities.getStagedChanges();

	entities.executeChanges(*this);
	if (eventHooks[svc_PacketEntities])
		eventHooks[svc_PacketEntities](&ent);

	endTime = std::chrono::high_resolution_clock::now();
	diffdTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	// std::clog << "[p] Time it took for packet executing was " << diffdTime.count() << "us, len: " << e.entity_data().length() << std::endl;
}

void DemoFile::handleUserMessage(UserMessage &e)
{
#define UserMessageSwitch(type) \
	case CS_UM_##type:	\
	{					\
		CCSUsrMsg_##type msg;		\
		if (msg.ParseFromString(e.msg_data()))										\
		{																			\
			std::string debug = e.DebugString();									\
			eventHooks[svc_UserMessage](&debug);									\
		}																			\
		else																		\
		{																			\
			std::cerr << "Error: parsing of " << #type << " failed" << std::endl;	\
			exit(1);																\
		}																			\
		break;																		\
	}
	if (!eventHooks[svc_UserMessage])
		return;

	switch (e.msg_type())
	{
		UserMessageSwitch(VGUIMenu);
		UserMessageSwitch(Geiger);
		UserMessageSwitch(Train);
		UserMessageSwitch(HudText);
		UserMessageSwitch(SayText);
		UserMessageSwitch(SayText2);
		UserMessageSwitch(TextMsg);
		UserMessageSwitch(HudMsg);
		UserMessageSwitch(ResetHud);
		UserMessageSwitch(GameTitle);
		UserMessageSwitch(Shake);
		UserMessageSwitch(Fade);
		UserMessageSwitch(Rumble);
		UserMessageSwitch(CloseCaption);
		UserMessageSwitch(CloseCaptionDirect);
		UserMessageSwitch(SendAudio);
		UserMessageSwitch(RawAudio);
		UserMessageSwitch(VoiceMask);
		UserMessageSwitch(RequestState);
		UserMessageSwitch(Damage);
		UserMessageSwitch(RadioText);
		UserMessageSwitch(HintText);
		UserMessageSwitch(KeyHintText);
		UserMessageSwitch(ProcessSpottedEntityUpdate);
		UserMessageSwitch(ReloadEffect);
		UserMessageSwitch(AdjustMoney);
		//UserMessageSwitch(UpdateTeamMoney);
		UserMessageSwitch(StopSpectatorMode);
		UserMessageSwitch(KillCam);
		UserMessageSwitch(DesiredTimescale);
		UserMessageSwitch(CurrentTimescale);
		UserMessageSwitch(AchievementEvent);
		UserMessageSwitch(MatchEndConditions);
		UserMessageSwitch(DisconnectToLobby);
		UserMessageSwitch(DisplayInventory);
		UserMessageSwitch(WarmupHasEnded);
		UserMessageSwitch(ClientInfo);
		UserMessageSwitch(CallVoteFailed);
		UserMessageSwitch(VoteStart);
		UserMessageSwitch(VotePass);
		UserMessageSwitch(VoteFailed);
		UserMessageSwitch(VoteSetup);
		UserMessageSwitch(SendLastKillerDamageToClient);
		UserMessageSwitch(ItemPickup);
		UserMessageSwitch(ShowMenu);
		UserMessageSwitch(BarTime);
		UserMessageSwitch(AmmoDenied);
		UserMessageSwitch(MarkAchievement);
		UserMessageSwitch(ItemDrop);
		UserMessageSwitch(GlowPropTurnOff);
	default:
		break;
	}
#undef UserMessageSwitch
}

void DemoFile::handleDataTable(DataTable &dt)
{
	for (size_t i = 0; i < dt.services.size(); i++)
	{
		dt.services[i].dataTable = dt.findSendTable(dt.services[i].nameDataTable);
		dt.services[i].flattenProps(dt);
	}
	dataTable.shallowSwap(dt);
	if (eventHooks[dem_datatables])
		eventHooks[dem_datatables](&dataTable);
}

void	DemoFile::create_metrics()
{
	extern StartupParser startupParameters;
#define HandleCase(type) \
	case svc_##type: \
	{			\
		handle##type(*(type *)pd.second); \
		break;	\
	}
#define HandleOther(type) \
	case svc_##type: \
	{ \
		if (eventHooks[svc_##type]) \
			eventHooks[svc_##type](&pd.second); \
		break; \
	}
#define HandleOtherNet(type) \
	case net_##type: \
	{ \
		if (eventHooks[net_##type]) \
			eventHooks[net_##type](&pd.second); \
		break; \
	}

	std::cout << "Frames: " << frames.size() << std::endl;
	for (; tick < frames.size(); tick++)
	{
		auto	startTime = std::chrono::high_resolution_clock::now();
		for (size_t x = 0; x < frames[tick].pckt.msg.size(); x++)
		{
			std::pair<int, void *> &pd = frames[tick].pckt.msg[x];

			switch (pd.first)
			{
				HandleCase(GameEventList);
				HandleCase(GameEvent);
				HandleCase(ServerInfo);
				HandleCase(CreateStringTable);
				HandleCase(UpdateStringTable);
				HandleCase(UserMessage);
				HandleCase(PacketEntities);
				HandleCase(DataTable);

				HandleOtherNet(Disconnect);
				HandleOtherNet(File);
				HandleOtherNet(Tick);
				HandleOtherNet(StringCmd);
				HandleOtherNet(SetConVar);
				HandleOtherNet(SignonState);

				HandleOther(ClassInfo);
				HandleOther(SetPause);
				HandleOther(VoiceInit);
				HandleOther(VoiceData);
				HandleOther(Print);
				HandleOther(Sounds);
				HandleOther(SetView);
				HandleOther(FixAngle);
				HandleOther(CrosshairAngle);
				HandleOther(BSPDecal);
				HandleOther(TempEntities);
				HandleOther(Prefetch);
				HandleOther(Menu);
				HandleOther(GetCvarValue);

			default:
				break;
			}
		}
		auto	endTime = std::chrono::high_resolution_clock::now();
		auto	diffdTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
		(void)diffdTime;
		// std::clog << "Time it took for frame " << i << " was " << diffdTime.count() << "us, uspp: " << \
		//			(diffdTime.count() == 0 ? 0 : (float)frames[i].pckt.msg.size() / (float)diffdTime.count() * 1000) \
		//			<< std::endl; 
	}
#undef HandleCase
#undef HandleOther
#undef HandleOtherNet
}


const size_t	DemoFile::getCurrentTick()
{
	return tick;
}