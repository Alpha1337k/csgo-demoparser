#include <demo.hpp>

void	getProtoMesssages(FileReader &f, int size, DemoFile &d)
{
#define ParseStatement(type, extra)														\
	case svc_##type:																	\
	{ 																					\
		if (s_##type.ParseFromArray(dataptr, length) == false)							\
		{ 																				\
			std::cerr << "Error: parsing failed: " << #type << std::endl;				\
			exit(1);																	\
		}																				\
		else if (parsemsg)																\
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl; 	\
		d.handle_packet((int)svc_##type, &s_##type);									\
		extra;																			\
		break;																			\
	}
#define	ParseNetStatement(type) 														\
	case net_##type: 																	\
	{																					\
		if (s_##type.ParseFromArray(dataptr, length) == false) 							\
		{																				\
			std::cerr << "Error: parsing failed: " << #type << std::endl;				\
			exit(1);																	\
		}																				\
		else if (parsemsg)																\
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl;	\
		d.handle_packet((int)net_##type, &s_##type);									\
		break;																			\
	}

	size_t startpos = f.getOffset();
	bool	parsemsg = startupParameters["--parsemsg"] == 0 ? false : true;

	/*
		yes this is the ugliest way to define static variables.
	*/
	static NOP s_NOP = NOP();
	static Disconnect s_Disconnect = Disconnect();
	static File s_File = File();
	static Tick s_Tick = Tick();
	static ClassInfo s_ClassInfo = ClassInfo();
	static StringCmd s_StringCmd = StringCmd();
	static SetConVar s_SetConVar = SetConVar();
	static SignonState s_SignonState = SignonState();
	static ServerInfo s_ServerInfo = ServerInfo();
	static SendTable s_SendTable = SendTable();
	static SetPause s_SetPause = SetPause();
	static CreateStringTable s_CreateStringTable = CreateStringTable();
	static UpdateStringTable s_UpdateStringTable = UpdateStringTable();
	static VoiceInit s_VoiceInit = VoiceInit();
	static VoiceData s_VoiceData = VoiceData();
	static Print s_Print = Print();
	static Sounds s_Sounds = Sounds();
	static SetView s_SetView = SetView();
	static FixAngle s_FixAngle = FixAngle();
	static CrosshairAngle s_CrosshairAngle = CrosshairAngle();
	static BSPDecal s_BSPDecal = BSPDecal();
	static UserMessage s_UserMessage = UserMessage();
	static GameEvent s_GameEvent = GameEvent();
	static PacketEntities s_PacketEntities = PacketEntities();
	static TempEntities s_TempEntities = TempEntities();
	static Prefetch s_Prefetch = Prefetch();
	static Menu s_Menu = Menu();
	static GameEventList s_GameEventList = GameEventList();
	static GetCvarValue s_GetCvarValue = GetCvarValue();

	while (f.getOffset() - startpos < (size_t)size || size == -1)
	{
		unsigned int	messagetype = f.readInt();
		unsigned int	length = f.readInt();

		char *dataptr = f.getCurrentChar();
		f.ForceIncrement(length);

		switch (messagetype)
		{
			ParseNetStatement(NOP);
			ParseNetStatement(Disconnect);
			ParseNetStatement(File);
			ParseNetStatement(Tick);
			ParseNetStatement(StringCmd);
			ParseNetStatement(SetConVar);
			ParseNetStatement(SignonState);
			ParseStatement(ServerInfo, );
			ParseStatement(SendTable, if (s_SendTable.is_end()) { return; });
			ParseStatement(ClassInfo,)
			ParseStatement(SetPause, );
			ParseStatement(CreateStringTable, );
			ParseStatement(UpdateStringTable, );
			ParseStatement(VoiceInit, );
			ParseStatement(VoiceData, );
			ParseStatement(Print, );
			ParseStatement(Sounds, );
			ParseStatement(SetView, );
			ParseStatement(FixAngle, );
			ParseStatement(CrosshairAngle, );
			ParseStatement(BSPDecal, );
			ParseStatement(UserMessage, );
			ParseStatement(GameEvent, );
			ParseStatement(PacketEntities, );
			ParseStatement(TempEntities, );
			ParseStatement(Prefetch, );
			ParseStatement(Menu, );
			ParseStatement(GameEventList, );
			ParseStatement(GetCvarValue, );
			default:
			{
				if (parsemsg)
					std::cerr << "Error: Could not find matching type " << messagetype  << " ,length: " << length << std::endl;
				break;
			}
		}

	}
#undef ParseStatement
#undef ParseNetStatement
}