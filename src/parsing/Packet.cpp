#include <demo.hpp>

Packet::Packet(FILE *f)
{

	fread(this, 1, sizeof(*this) - sizeof(MessageVector), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	unsigned int chunkSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);
	fread(&chunkSize, sizeof(int), 1, f);
	msg = getProtoMesssages(f, chunkSize);
}

std::ostream &operator<<(std::ostream &o, const Packet &p)
{
	o << "Packet {\n";
	o << "flags:            " << p.splits[0].flags << std::endl;
	// o << "viewOrigin:       " << p.splits[0].viewOrigin << std::endl;
	// o << "viewAngles:       " << p.splits[0].viewAngles << std::endl;
	// o << "localViewAngles:  " << p.splits[0].localViewAngles << std::endl;
	// o << "viewOrigin:       " << p.splits[0].viewOrigin2 << std::endl;
	// o << "viewAngles:       " << p.splits[0].viewAngles2 << std::endl;
	// o << "localViewAngles:  " << p.splits[0].localViewAngles2 << std::endl;
	// o << "flags:            " << p.splits[1].flags << std::endl;
	// o << "viewOrigin:       " << p.splits[1].viewOrigin << std::endl;
	// o << "viewAngles:       " << p.splits[1].viewAngles << std::endl;
	// o << "localViewAngles:  " << p.splits[1].localViewAngles << std::endl;
	// o << "viewOrigin:       " << p.splits[1].viewOrigin2 << std::endl;
	// o << "viewAngles:       " << p.splits[1].viewAngles2 << std::endl;
	// o << "localViewAngles:  " << p.splits[1].localViewAngles2 << std::endl;
	o << "Messages: (" << p.msg.size() << ") [ \n";
#define MessagePrinter(type) \
	{	\
	std::cout << "packet: " << #type << std::endl; \
	break;	\
	}
	for (size_t i = 0; i < p.msg.size(); i++)
	{
		switch (p.msg[i].first)
		{
			case MSG_SERVER_INFO:
				MessagePrinter(ServerInfo);
			case MSG_CREATE_STRING_TABLE:
				MessagePrinter(CreateStringTable);
			case MSG_UPDATE_STRING_TABLE:
				MessagePrinter(UpdateStringTable);
			case MSG_USER_MESSAGE:
				MessagePrinter(UserMessage);
			case MSG_GAME_EVENT:
				MessagePrinter(GameEvent);
			case MSG_PACKET_ENTITIES:
				MessagePrinter(PacketEntities);
			case MSG_GAME_EVENTS_LIST:
				MessagePrinter(GameEventList);
			case MSG_DATA_TABLE:
				MessagePrinter(SendTable);
			default:
				MessagePrinter(NULL);
		}
	}
#undef MessagePrinter
	o << "] }";

	return (o);
}