#include <demo.hpp>

void	DemoFile::handleGameEventList(GameEventList &ge)
{
	gEvents.reserve(ge.descriptors_size());
	for (size_t i = 0; i < ge.descriptors_size(); i++)
	{
		const GameEventList_descriptor_t &dsc = ge.descriptors(i);
		gEvents.push_back(dsc);
	}
}

void	DemoFile::handleGameEvent(GameEvent &ge)
{
#define SwitchPrint(val, toprint) \
	case val: \
	{			\
		std::cout << eventKey.toprint(); \
		break;	\
	}
	if (ge.eventid() >= gEvents.size())
		throw std::overflow_error("Eventid is more than gEvents");
	const GameEventList_descriptor_t &event = gEvents[ge.eventid()];
	std::cout << "Event " << event.name() << ":{ ";
	for (size_t i = 0; i < event.keys_size(); i++)
	{
		const GameEventList_key_t &key = event.keys(i);
		const GameEvent_key_t &eventKey = ge.keys(i);
		std::cout << key.name() << ": ";
		if (key.name() == "userid")
		{
			for (size_t i = 0; i < players.size(); i++)
			{
				if (players[i].userId == eventKey.val_short())
				{
					std::cout << players[i].userName;
					break;
				}
			}			
		}
		else
		{
			switch (eventKey.type())
			{
				SwitchPrint(1, val_string);
				SwitchPrint(2, val_float);
				SwitchPrint(3, val_long);
				SwitchPrint(4, val_short);
				SwitchPrint(5, val_byte);
				SwitchPrint(6, val_bool);
				SwitchPrint(7, val_uint64);
				SwitchPrint(8, val_wstring);
			default:
				break;
			}
		}
			
		std::cout << ", ";
	}
	std::cout << "}\n";
#undef SwitchPrint
}

void DemoFile::handleServerInfo(ServerInfo &si)
{
	info = &si;
	std::cout << "ServerInfo: {" << std::endl;
	std::cout << si.DebugString() << std::endl;
	std::cout << "}" << std::endl;
}

void DemoFile::handleCreateStringTable(CreateStringTable &si)
{
	std::cout << "CreateStringTable: { name: " << si.name() << ", stringdata len:" << si.string_data().length() << " }" << std::endl;
	parsedTables.push_back(ParsedStringTable(si, *this));
}

void DemoFile::handleUpdateStringTable(UpdateStringTable &si)
{
	if (si.table_id() >= parsedTables.size())
		return;
	const std::string &tableName = parsedTables[si.table_id()].origin.name();
	std::cout << "UpdateStringTable: { name: " << tableName << ", changed: " << si.num_changed_entries() << ", length: " << si.string_data().length() << "}" << std::endl;

	ParsedStringTable &target = parsedTables[si.table_id()];
	if (tableName == "userinfo")
	{
		target.Update(si, *this, true);
	}
}

void DemoFile::handlePacketEntities(PacketEntities &e)
{
	entities.parse(e, dataTable);
	std::cout << "PacketEntities: { updated_entries: " << e.updated_entries() << ", data_len: " << e.entity_data().length() << "}" << std::endl; 
}

template < class T >
void printUserMessage(UserMessage &e, std::string type)
{
	T msg;

	if (msg.ParseFromString(e.msg_data()))
	{
		std::cout << "UserMessage: "<< type << ":{\n" << msg.DebugString() << "}"<< std::endl;
	}
	else
	{
		std::cerr << "Error: parsing of " << type << " failed" << std::endl;
		exit(0);
	}
}

void DemoFile::handleUserMessage(UserMessage &e)
{
#define UserMessageSwitch(type) \
	case CS_UM_##type: \
		printUserMessage<CCSUsrMsg_##type>(e, #type); \
		break;

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
	for (size_t i = 0; i < dt.msg.size(); i++)
	{
		if (dt.msg[i].first != svc_SendTable)
			std::cerr << "Error: weird item in datatable: " << dt.msg[i].first << std::endl;
		else
		{
			std::cout << "SendTable: name: " << ((SendTable *)dt.msg[i].second)->net_table_name() << std::endl;
		}
	}
	for (size_t i = 0; i < dt.services.size(); i++)
	{
		dt.services[i].dataTable = dt.findSendTable(dt.services[i].nameDataTable);
		dt.services[i].flattenProps(dt);
	}
	dataTable = dt;
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
		if (startupParameters["-v"]) \
			std::cout << #type << ": " << ((type *)pd.second)->DebugString() << std::endl; \
		else if (startupParameters["-d"]) \
			std::cout << #type << std::endl; \
		break; \
	}
#define HandleOtherNet(type) \
	case net_##type: \
	{ \
		if (startupParameters["-v"]) \
			std::cout << #type << ": " << ((type *)pd.second)->DebugString() << std::endl; \
		else if (startupParameters["-d"]) \
			std::cout << #type << std::endl; \
		break; \
	}

	for (size_t i = 0; i < frames.size(); i++)
	{
		for (size_t x = 0; x < frames[i].pckt.msg.size(); x++)
		{
			const std::pair<int, void *> &pd = frames[i].pckt.msg[x];

			switch (pd.first)
			{
				HandleCase(GameEventList);
				HandleCase(GameEvent);
				HandleCase(ServerInfo);
				HandleCase(CreateStringTable);
				HandleCase(UpdateStringTable);
				HandleCase(UserMessage);
				HandleCase(DataTable);
				HandleCase(PacketEntities);

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
	}
#undef HandleCase
#undef HandleOther
#undef HandleOtherNet
}
