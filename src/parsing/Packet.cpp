#include <demo.hpp>

//109269

Packet::Packet(FILE *f)
{

	fread(this, 1, sizeof(*this) - sizeof(MessageVector), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	int chunkSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);

	fread(&chunkSize, sizeof(int), 1, f);
	std::cout << "Chunksize: " << chunkSize << std::endl;

	msg = getProtoMesssages(f, chunkSize);
}

Packet::Packet() {}

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
			case svc_ServerInfo:
				MessagePrinter(ServerInfo);
			case svc_CreateStringTable:
				MessagePrinter(CreateStringTable);
			case svc_UpdateStringTable:
				MessagePrinter(UpdateStringTable);
			case svc_UserMessage:
				MessagePrinter(UserMessage);
			case svc_GameEvent:
				MessagePrinter(GameEvent);
			case svc_PacketEntities:
				MessagePrinter(PacketEntities);
			case svc_GameEventList:
				MessagePrinter(GameEventList);
			case svc_SendTable:
				MessagePrinter(SendTable);
			default:
				MessagePrinter(NULL);
		}
	}
#undef MessagePrinter
	o << "] }";

	return (o);
}