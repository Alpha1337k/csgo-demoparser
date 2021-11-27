#include <demo.hpp>

std::ostream &operator<<(std::ostream &o, const Player &p)
{
	o << "Player: {" << std::endl;
	o << "version: " << p.version << std::endl;
	o << "xUid: " << p.xUid << std::endl;
	o << "userName: " << p.userName << std::endl;
	o << "userId: " << p.userId << std::endl;
	o << "gUid: " << p.gUid << std::endl;
	o << "friendsId: " << p.friendsId << std::endl;
	o << "friendsName: " << p.friendsName << std::endl;
	o << "isFake: " << p.isFake << std::endl;
	o << "isHltv: " << p.isHltv << std::endl;
	o << "customFiles: " << p.friendsName << std::endl;
	o << "filesDownloaded: " << (unsigned int)p.filesDownloaded << std::endl;
	o << "}" << std::endl;

	return o;
}

std::ostream &operator<<(std::ostream &o, const Vector &d)
{
	o << "[ " << d.x << ", " << d.y << ", " << d.z << " ]";
	return o;
}

std::ostream &operator<<(std::ostream &o, const Vector2 &d)
{
	o << "[ " << d.x << ", " << d.y << " ]";
	return o;
}

std::ostream &operator<<(std::ostream &o, const DemHeader &d)
{
	o << "DemHeader {\n";
	o << "headerDef:       " << d.headerDef << std::endl;
	o << "demoProtocol:    " << d.demoProtocol << std::endl;
	o << "networkProtocol: " << d.networkProtocol << std::endl;
	o << "serverName:      " << d.serverName << std::endl;
	o << "clientName:      " << d.clientName << std::endl;
	o << "mapName:         " << d.mapName << std::endl;
	o << "gameDir:         " << d.gameDir << std::endl;
	o << "demoLength:      " << d.demoLength << std::endl;
	o << "ticks:           " << d.ticks << std::endl;
	o << "frames:          " << d.frames << std::endl;
	o << "signOnLength:    " << d.signOnLength << std::endl;

	o << "}\n";

	return (o);
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

std::ostream &operator<<(std::ostream &o, const PropW &p)
{

	o << p.prop << std::endl;
	return (o);
}

std::ostream &operator<<(std::ostream &o, const SendTable_sendprop_t &prop)
{
#define PrintVariable(name, var) std::cout << ", " << name << ": " << var;

	o << "Prop: { ";
	PrintVariable("type", prop.type());
	PrintVariable("name", prop.var_name());
	PrintVariable("flags", prop.flags());
	PrintVariable("priority", prop.priority());
	PrintVariable("dt_name", prop.dt_name());
	PrintVariable("elements", prop.num_elements());
	PrintVariable("low_value", prop.low_value());
	PrintVariable("high_value", prop.high_value());
	PrintVariable("num_bits", prop.num_bits());
	o << "}\n";

	return (o);
#undef PrintVariable
}