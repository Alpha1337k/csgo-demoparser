#include <demo.hpp>

std::ostream &operator<<(std::ostream &o, const Player &p)
{
	o << "Player: {" << std::endl;
	o << "version: " << p.md.version << std::endl;
	o << "xUid: " << p.md.xUid << std::endl;
	o << "userName: " << p.md.userName << std::endl;
	o << "userId: " << p.md.userId << std::endl;
	o << "gUid: " << p.md.gUid << std::endl;
	o << "friendsId: " << p.md.friendsId << std::endl;
	o << "friendsName: " << p.md.friendsName << std::endl;
	o << "isFake: " << p.md.isFake << std::endl;
	o << "isHltv: " << p.md.isHltv << std::endl;
	o << "customFiles: " << p.md.customFiles << std::endl;
	o << "filesDownloaded: " << (unsigned int)p.md.filesDownloaded << std::endl;
	if (p.packetRef)
		o << "packetData: " << *p.packetRef << std::endl;
	else
		o << "packetData: 0" << std::endl;
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

std::ostream &operator<<(std::ostream &o, const PropW &p)
{
	o << "Name: " << p.path << "\t";
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
	o << "}";

	return (o);
#undef PrintVariable
}

std::ostream &operator<<(std::ostream &o, const DataTable::ServiceClass &p)
{
#define PrintVariable(name, var) o << ", " << name << ": " << var;

	o << "ServiceClass: { ";
	PrintVariable("id", p.id);
	PrintVariable("name", p.name);
	PrintVariable("tableName", p.nameDataTable);
	o << "Props:";
	for (size_t i = 0; i < p.props.size(); i++)
	{
		o << p.props[i] << std::endl;
	}
	
	o << "}";

	return (o);
#undef PrintVariable	
}

std::ostream &operator<<(std::ostream &o, const Frame &f)
{
	o << "Frame {\n";
	o << "cmd:        " << (int)f.cmd << std::endl;
	o << "tick:       " << f.tick << std::endl;
	o << "playerslot: " << (int)f.playerslot << std::endl;
	// o << "packet: {\n" << f.pckt << std::endl; 
	o << "} }\n";

	return (o);
}

std::ostream &operator<<(std::ostream &o, const GameEntities::Property &p)
{
	switch (p.type)
	{
		case decoded_int:
			o << std::get<int>(p.data);
			break;
		case decoded_float:
			o << std::get<float>(p.data);
			break;
		case decoded_Vector:
			o << std::get<Vector>(p.data);
			break;
		case decoded_Vector2:
			o << std::get<Vector2>(p.data);
			break;
		case decoded_string:
			o << std::get<std::string>(p.data);
			break;
		case decoded_array:
		{
			const std::vector<GameEntities::Property> &data = std::get<std::vector<GameEntities::Property> >(p.data);
			o << "[ ";
			for (size_t i = 0; i < data.size(); i++)
			{
				o << data[i] << ",";
			}
			o << " ]";
			
		}
	}
	return o;
}

std::ostream &operator<<(std::ostream &o, const GameEntities::Entity &e)
{
	if (!e.parentService)
		return o;
	o << "Packet: " << e.parentService->nameDataTable << std::endl;

	for (size_t i = 0; i < e.properties.size(); i++)
	{
		const std::pair<const std::string *, GameEntities::Property> &it = e.properties[i];
		if (!it.first)
			continue;
		o << *it.first << ": " << it.second << '\n';
	}
	return (o);
}