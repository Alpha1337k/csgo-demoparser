#include <demo.hpp>

void	getProtoMesssages(FileReader &f, int size, MessageVector &messages)
{
	extern StartupParser startupParameters;
#define ParseStatement(type, extra) \
	case svc_##type: \
	{ \
		type *svi = new type; \
		if (svi->ParseFromArray(dataptr, length) == false) \
		{ \
			std::cerr << "Error: parsing failed: " << #type << std::endl; \
			svi = 0; \
			exit(1); \
		} \
		else if (parsemsg) \
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl; \
		messages.push_back(std::make_pair(svc_##type, svi)); \
		/* std::cout << "{" << svi->DebugString() << "}" << std::endl; */ \
		extra \
		break; \
	}

#define	ParseNetStatement(type) \
	case net_##type: \
	{ \
		type *net = new type; \
		if (net->ParseFromArray(dataptr, length) == false) \
		{ \
			std::cerr << "Error: parsing failed: " << #type << std::endl; \
			net = 0; \
			exit(1); \
		} \
		else if (parsemsg) \
			std::cout << "Parsed "  << #type << ", length: " << length << std::endl; \
		messages.push_back(std::make_pair(net_##type, net)); \
		/* std::cout << "{" << net->DebugString() << "}" << std::endl; */ \
		break; \
	}

	size_t startpos = f.getOffset();
	bool	parsemsg = startupParameters["--parsemsg"] == 0 ? false : true;

	while (f.getOffset() - startpos < (size_t)size || size == -1)
	{
		unsigned int	messagetype = f.readInt();
		unsigned int	length = f.readInt();

		//std::cout << "Iter: " << iter << " messagetype:" << (int)messagetype << ", length: " << length << std::endl;

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
			ParseStatement(SendTable, if (svi->is_end()) {return ;});
			ParseStatement(ClassInfo, )
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