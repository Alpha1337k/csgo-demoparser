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
		eventKey.val_string();
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
	sTables.push_back(si);
	// if (si.name() == "userinfo")
	// {

	// }
}

void DemoFile::handleUpdateStringTable(UpdateStringTable &si)
{
	if (si.table_id() >= sTables.size())
		return;
	std::cout << "UpdateStringTable: { id: " << si.table_id() << ", changed: " << si.num_changed_entries() << "}" << std::endl;
	CreateStringTable &target = sTables[si.table_id()];


}

void DemoFile::handlePacketEntities(PacketEntities &e)
{

}

template < class T >
void printUserMessage(UserMessage &e, std::string type)
{
	T msg;

	if (msg.ParseFromString(e.msg_data()))
	{
		std::cout << type << ":{\n" << msg.DebugString() << "}"<< std::endl;
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


void	DemoFile::create_metrics()
{
#define HandleCase(value, type) \
	case value: \
	{			\
		handle##type(*(type *)pd.second); \
		break;	\
	}			\

	for (size_t i = 0; i < frames.size(); i++)
	{
		if (i > 0 && frames[i].tick > 0 && frames[i-1].tick < 0)
			std::cout << "\n\n---------- Start of game -------------\n" << std::endl; 
		if (frames[i].cmd == dem_signon || frames[i].cmd == dem_packet)
		{
			for (size_t x = 0; x < frames[i].pckt.msg.size(); x++)
			{
				const std::pair<PacketTypes, void *> &pd = frames[i].pckt.msg[x];

				switch (pd.first)
				{
					HandleCase(MSG_GAME_EVENTS_LIST, GameEventList);
					HandleCase(MSG_GAME_EVENT, GameEvent);
					HandleCase(MSG_SERVER_INFO, ServerInfo);
					HandleCase(MSG_CREATE_STRING_TABLE, CreateStringTable);
					HandleCase(MSG_UPDATE_STRING_TABLE, UpdateStringTable);
					HandleCase(MSG_USER_MESSAGE, UserMessage);
				
				default:
					break;
				}
			}

		}
	}
#undef HandleCase
}