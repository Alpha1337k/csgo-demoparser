#include <demo.hpp>

StartupParser startupParameters;
DemoFile	*demoref;


void	printServerInfo(void *data)
{
	ServerInfo *si = (ServerInfo *)data;

	std::cout << si->DebugString() << std::endl;
}

void	printGameEvent(void *data)
{
#define SwitchPrint(val, toprint)			\
	case val:								\
	{										\
		std::cout << eventKey.toprint();	\
		break;								\
	}

	GameEvent &ge = *(GameEvent *)data;
	const GameEventList_descriptor_t &event = demoref->getGameEvent(ge.eventid());

	std::cout << "Event " << event.name() << ":{ ";
	for (int i = 0; i < event.keys_size(); i++)
	{
		const GameEventList_key_t &key = event.keys(i);
		const GameEvent_key_t &eventKey = ge.keys(i);

		std::cout << key.name() << ": ";
		{
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
		}
		std::cout << ", ";
	}
	std::cout << "}\n";	
}

void	printPacketEntities(void *data)
{
#define PrintVariable(name, var, type) std::cout << ", \"" << name << "\": " << std::get<type>(var);
#define PrintExists(property, cast, name, target)									\
		{	\
			const GameEntities::Property *prop = target.getProperty(property);	\
			if (prop != 0)													\
				PrintVariable(name, prop->data, cast);						\
		}

	static size_t c = 0;

	const std::unordered_map<int, Player> &players = demoref->getPlayers();
	
	if (c == 0)
		std::cout << '[';
	else
		std::cout << ",[";
	c++;

	for (auto i = players.begin(); i != players.end();)
	{
		const Player &pl = i->second;

		std::cout << "{ \"name\": \"" << pl.md.userName << '"';
		if (pl.packetRef != 0)
		{
			PrintExists("m_iHealth", int, "health", pl);
			PrintExists("m_iAccount", int, "balance", pl);
			PrintExists("m_bHasFelmet", int, "hasHelmet", pl);
			PrintExists("localdata.m_vecOrigin[0]", int, "lVecOriginZ", pl);
			PrintExists("csnonlocaldata.m_vecOrigin", Vector2, "nlVecOriginXY", pl);
			PrintExists("m_angEyeAngles[0]", float, "eyeAnglePitch", pl);
			PrintExists("m_angEyeAngles[1]", float, "eyeAngleYaw", pl);
			PrintExists("m_iTeamNum", int, "teamId", pl);
		}
		i++;
		if (i != players.end())
			std::cout << " },\n";
		else
			std::cout << " }\n";
	}
	std::cout << "]\n";
	// static int count = 0;
	// count++;
	// if (count > 1000)
	// {
	// 	auto data = demoref->getEntities();

	// 	for (auto it = data.begin(); it != data.end(); it++)
	// 	{
	// 		std::cout << demoref->getEntity(it->second) << "\n";
	// 	}
	// 	std::cout << std::endl;
	// }

#undef PrintVariable
}

void	printCreateStringTable(void *data)
{
	CreateStringTable *st = (CreateStringTable *)data;

	if (st->name() != "userinfo")
		return;

}

void	printDataTable(void *d)
{
	DataTable *data = (DataTable *)d;

	for (size_t i = 0; i < data->services.size(); i++)
	{
		std::cout << data->services[i] << std::endl;
	}
	exit(-1);	
}

void	printTime(std::string name, std::chrono::system_clock::time_point p1, std::chrono::system_clock::time_point p2)
{
	auto	diffdTime = std::chrono::duration_cast<std::chrono::microseconds>(p1 - p2);
	(void)diffdTime;
	std::cerr << name << " took " << diffdTime.count() / 1000 << "ms" << std::endl;
}

int main(int argc, char **argv, char **env)
{
	(void)env;
	auto	startTime = std::chrono::high_resolution_clock::now();

	startupParameters = StartupParser(argc, argv);
	if (argc < 2)
	{
		std::cerr << "Error: filepath required" << std::endl;
		return (-2);
	}

	FileReader f;

	if (startupParameters.last().first == "-stdin")
	{
		f.loadFd(0);
	}
	else if (f.load(startupParameters.last().first) == -1)
	{
		std::cerr << "Error: file could not be opened" << std::endl;
		return (-1);
	}
	auto	startEnd = std::chrono::high_resolution_clock::now();
	printTime("Loading", startEnd, startTime);

	DemoFile demo;
	demoref = &demo;

	// demo.addEventHook(svc_PacketEntities, printPacketEntities);

	// std::cout << "[";

	demo.start_parse(f);
	// std::cout << "]" << std::endl;
	auto	parseTime = std::chrono::high_resolution_clock::now();
	printTime("Parsing", parseTime, startEnd);

	google::protobuf::ShutdownProtobufLibrary();
	std::cerr << "Done!" << std::endl;
	return (0);
}