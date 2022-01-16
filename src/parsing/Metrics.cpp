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
	entities.parse(e, dataTable, *this);

	if (eventHooks[svc_PacketEntities])
		eventHooks[svc_PacketEntities](&e);
}

void DemoFile::handleUserMessage(UserMessage &e)
{
#define UserMessageSwitch(type)														\
	case CS_UM_##type:																\
	{																				\
		CCSUsrMsg_##type msg;														\
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
		//UserMessageSwitch(UpdateTeamMoney); broken??
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

void DemoFile::handleDataTable()
{
	for (size_t i = 0; i < dataTable.services.size(); i++)
	{
		dataTable.services[i].dataTable = &dataTable.msg[dataTable.services[i].id];
		dataTable.services[i].flattenProps(dataTable);
	}

	if (eventHooks[dem_datatables])
		eventHooks[dem_datatables](&dataTable);
}

void DemoFile::handleSendTable(SendTable &st)
{
	dataTable.msg.push_back(st);
	if (eventHooks[dem_datatables])
		eventHooks[dem_datatables](&dataTable);
}


void	DemoFile::handle_packet(int type, void *data)
{
#define HandleCase(type)						\
	case svc_##type:							\
	{											\
		handle##type(*(type *)data);			\
		break;									\
	}
#define HandleOther(type)						\
	case svc_##type:							\
	{											\
		if (eventHooks[svc_##type])				\
			eventHooks[svc_##type](data);		\
		break;									\
	}
#define HandleOtherNet(type)					\
	case net_##type:							\
	{											\
		if (eventHooks[net_##type])				\
			eventHooks[net_##type](data); \
		break;									\
	}

	switch (type)
	{
		HandleCase(GameEventList);
		HandleCase(GameEvent);
		HandleCase(ServerInfo);
		HandleCase(CreateStringTable);
		HandleCase(UpdateStringTable);
		HandleCase(UserMessage);
		HandleCase(PacketEntities);
		HandleCase(SendTable);

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
		std::cerr << "Error: unknown" << type << std::endl;
		break;
	}

#undef HandleCase
#undef HandleOther
#undef HandleOtherNet
}
