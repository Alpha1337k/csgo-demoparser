#include <demo.hpp>

MessageVector	getProtoMesssages(FILE *f, int size)
{
#define ParseStatement(type) \
	type *svi = new type; \
	if (svi->ParseFromString(packetdata) == false) \
	{ \
		std::cerr << "Error: parsing failed: " << #type << std::endl; \
		svi = 0; \
	}
#define AddStatement(type, data) messages.push_back(std::make_pair(type, data));

	size_t iter = 0;
	MessageVector	messages;

	while (iter < (size_t)size || size == -1)
	{
		unsigned int	messagetype = readVarInt(f, &iter);
		unsigned int	length = readVarInt(f, &iter);;

		//std::cout << "Iter: " << iter << " messagetype:" << (int)messagetype << std::endl;
		//std::cout << "Length: " << length << std::endl;

		std::string packetdata;
		packetdata.resize(length);

		iter += fread(&packetdata[0], 1, length, f);

		switch (messagetype)
		{
		case MSG_SERVER_INFO:
			{
				ParseStatement(ServerInfo);
				// std::cout << "ServerInfo: {\n" << svi->DebugString() << "\n}\n";
				AddStatement(MSG_SERVER_INFO, svi)
				break;
			}
		case MSG_CREATE_STRING_TABLE:
			{
				ParseStatement(CreateStringTable);
				// std::cout << "CreateTable: {\n" << svi->DebugString() << "\n}\n";
				// std::cout << "Created table named " << svi->name() << std::endl;
				AddStatement(MSG_CREATE_STRING_TABLE, svi)
				break;
			}
		case MSG_UPDATE_STRING_TABLE:
			{
				ParseStatement(UpdateStringTable);
				// std::cout << "UpdateTable: {\n" << svi->DebugString() << "\n}\n";
				// std::cout << "Updated table with id " << svi->table_id() << std::endl;
				AddStatement(MSG_UPDATE_STRING_TABLE, svi)
				break;
			}
		case MSG_USER_MESSAGE:
			{
				ParseStatement(UserMessage);
				// std::cout << "UserMessage: {\n" << svi->DebugString() << "\n}\n";
				AddStatement(MSG_USER_MESSAGE, svi)
				break;
			}
		case MSG_GAME_EVENT:
			{
				ParseStatement(GameEvent);
				// std::cout << "GameEvent: {\n" << svi->DebugString() << "\n}\n";
				AddStatement(MSG_GAME_EVENT, svi)
				GameEventParsed e(*svi);
				break;
			}
		case MSG_PACKET_ENTITIES:
			{
				ParseStatement(PacketEntities);
				// std::cout << "PacketEntities: {\n" << svi->DebugString() << "\n}\n";
				AddStatement(MSG_PACKET_ENTITIES, svi)
				break;
			}
		case MSG_GAME_EVENTS_LIST:
			{
				ParseStatement(GameEventList);
				// std::cout << "GameEventList: {\n" << svi->DebugString() << "\n}\n";
				AddStatement(MSG_GAME_EVENTS_LIST, svi)
				// GameEventListParsed p(*svi);
				break;
			}
		case MSG_DATA_TABLE:
			{
				ParseStatement(SendTable);
				// std::cout << "DataTable: { size:" << svi->is_end() << ", " << length << "}\n";
				if (svi->is_end())
					return messages;
				AddStatement(MSG_DATA_TABLE, svi)
				break;
			}
		default:
			{
				// std::cerr << "Error: Could not find matching type " << messagetype  << "Length: " << length << std::endl;
			
				break;
			}
		}

	}
#undef AddStatement
#undef ParseStatement
	return messages;
}