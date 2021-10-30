#include <demo.hpp>

Packet::Packet(FILE *f)
{
	//viewOrigin.Init();
	//viewOrigin2.Init();
	//localViewAngles.Init();
	//localViewAngles2.Init();
	//viewAngles.Init();
	//viewAngles2.Init();

	fread(this, 1, sizeof(*this), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	unsigned int chunkSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);
	//std::cout << "sequence: " << sequenceIn << ", " << sequenceOut << std::endl;
	fread(&chunkSize, sizeof(int), 1, f);
	std::cout << "chunkSize: " << chunkSize << std::endl;
	size_t iter = 0;
	while (iter < chunkSize)
	{
		unsigned int	messagetype = readVarInt(f, &iter);
		unsigned int	length = readVarInt(f, &iter);;

		//std::cout << "Iter: " << iter << " messagetype:" << (int)messagetype << std::endl;
		//std::cout << "Length: " << length << std::endl;

		std::string packetdata;
		packetdata.resize(length);

		iter += fread(&packetdata[0], 1, length, f);

#define ParseStatement if (svi.ParseFromString(packetdata) == false) {std::cerr << "Error: parsing failed" << std::endl;}

		switch (messagetype)
		{
		case MSG_SERVER_INFO:
			{
				ServerInfo svi;
				ParseStatement
				std::cout << "ServerInfo: {\n" << svi.DebugString() << "\n}\n";
				break;
			}
		case MSG_CREATE_STRING_TABLE:
			{
				CreateStringTable svi;
				ParseStatement
				//std::cout << "CreateTable: {\n" << svi.DebugString() << "\n}\n";
				std::cout << "Created table named " << svi.name() << std::endl;
				break;
			}
		case MSG_UPDATE_STRING_TABLE:
			{
				UpdateStringTable svi;
				ParseStatement
				std::cout << "UpdateTable: {\n" << svi.DebugString() << "\n}\n";
				std::cout << "Updated table with id " << svi.table_id() << std::endl;
				break;
			}
		case MSG_USER_MESSAGE:
			{
				UserMessage svi;
				ParseStatement
				std::cout << "UserMessage: {\n" << svi.DebugString() << "\n}\n";
				break;
			}
		case MSG_GAME_EVENT:
			{
				GameEvent svi;
				ParseStatement
				std::cout << "GameEvent: {\n" << svi.DebugString() << "\n}\n";
				break;
			}
		case MSG_PACKET_ENTITIES:
			{
				PacketEntities svi;
				ParseStatement
				std::cout << "PacketEntities: {\n" << svi.DebugString() << "\n}\n";
				break;
			}
		case MSG_GAME_EVENTS_LIST:
			{
				GameEventList svi;
				ParseStatement
				std::cout << "GameEventList: {\n" << svi.DebugString() << "\n}\n";
				break;
			}
		case MSG_DATA_TABLE:
			{
				SendTable svi;
				ParseStatement
				std::cout << "DataTable: {\n" << svi.DebugString() << "\n}\n";
				break;				
			}
		default:
			break;
		}

	}
#undef ParseStatement	
	 
}

std::ostream &operator<<(std::ostream &o, const Packet &p)
{
	o << "Packet {\n";
	o << "flags:            " << p.splits[0].flags << std::endl;
	o << "viewOrigin:       " << p.splits[0].viewOrigin << std::endl;
	o << "viewAngles:       " << p.splits[0].viewAngles << std::endl;
	o << "localViewAngles:  " << p.splits[0].localViewAngles << std::endl;
	o << "viewOrigin:       " << p.splits[0].viewOrigin2 << std::endl;
	o << "viewAngles:       " << p.splits[0].viewAngles2 << std::endl;
	o << "localViewAngles:  " << p.splits[0].localViewAngles2 << std::endl;
	o << "flags:            " << p.splits[1].flags << std::endl;
	o << "viewOrigin:       " << p.splits[1].viewOrigin << std::endl;
	o << "viewAngles:       " << p.splits[1].viewAngles << std::endl;
	o << "localViewAngles:  " << p.splits[1].localViewAngles << std::endl;
	o << "viewOrigin:       " << p.splits[1].viewOrigin2 << std::endl;
	o << "viewAngles:       " << p.splits[1].viewAngles2 << std::endl;
	o << "localViewAngles:  " << p.splits[1].localViewAngles2 << std::endl;
	o << "}\n";

	return (o);
}