#include <demo.hpp>


MessageVector	getProtoMesssages(FILE *f, int size)
{
	extern StartupParser startupParameters;
	size_t iter = 0;
	MessageVector	messages;

#define ParseStatement(type) \
	case svc_##type: \
	{ \
		type *svi = new type; \
		if (svi->ParseFromString(packetdata) == false) \
		{ \
			std::cerr << "Error: parsing failed: " << #type << std::endl; \
			svi = 0; \
			exit(1); \
		} \
		else if (startupParameters["--parsemsg"]) \
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl; \
		messages.push_back(std::make_pair(svc_##type, svi)); \
		std::cout << "{" << svi->DebugString() << "}" << std::endl; \
		break; \
	}

#define	ParseNetStatement(type) \
	case net_##type: \
	{ \
		type *net = new type; \
		if (net->ParseFromString(packetdata) == false) \
		{ \
			std::cerr << "Error: parsing failed: " << #type << std::endl; \
			net = 0; \
			exit(1); \
		} \
		else if (startupParameters["--parsemsg"]) \
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl; \
		messages.push_back(std::make_pair(net_##type, net)); \
		std::cout << net->DebugString() << std::endl; \
		break; \
	}

	while (iter < (size_t)size || size == -1)
	{
		unsigned int	messagetype = readVarInt(f, &iter);
		unsigned int	length = readVarInt(f, &iter);

		std::cout << "Iter: " << iter << " messagetype:" << (int)messagetype << ", length: " << length << std::endl;

		std::string packetdata;
		packetdata.resize(length);

		iter += fread(&packetdata[0], 1, length, f);

		switch (messagetype)
		{
			ParseNetStatement(NOP);
			ParseNetStatement(Disconnect);
			ParseNetStatement(File);
			ParseNetStatement(Tick);
			ParseNetStatement(StringCmd);
			ParseNetStatement(SetConVar);
			ParseNetStatement(SignonState);
			ParseStatement(ServerInfo);
			ParseStatement(SendTable);
			ParseStatement(ClassInfo)
			ParseStatement(SetPause);
			ParseStatement(CreateStringTable);
			ParseStatement(UpdateStringTable);
			ParseStatement(VoiceInit);
			ParseStatement(VoiceData);
			ParseStatement(Print);
			ParseStatement(Sounds);
			ParseStatement(SetView);
			ParseStatement(FixAngle);
			ParseStatement(CrosshairAngle);
			ParseStatement(BSPDecal);
			ParseStatement(UserMessage);
			ParseStatement(GameEvent);
			ParseStatement(PacketEntities);
			ParseStatement(TempEntities);
			ParseStatement(Prefetch);
			ParseStatement(Menu);
			ParseStatement(GameEventList);
			ParseStatement(GetCvarValue);
			default:
			{
				if (messagetype == 0)
					return messages;
				std::cerr << "Error: Could not find matching type " << messagetype  << " ,length: " << length << std::endl;
				break;
			}
		}

	}
#undef ParseStatement
#undef ParseNetStatement
	return messages;
}