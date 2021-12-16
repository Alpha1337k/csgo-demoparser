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
#define SwitchPrint(val, toprint) \
	case val: \
	{			\
		std::cout << eventKey.toprint(); \
		break;	\
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
#define PrintVariable(name, var) std::cout << ", \"" << name << "\": " << var;
#define PrintExists(property, cast, name) \
		{	\
			const GameEntities::Property *prop = pl.getProperty(property); \
			if (prop != 0) \
				PrintVariable(name, *(cast *)prop->data); \
		}

	static int c = 0;
	std::vector<GameEntities::StagedChange *> *v = (std::vector<GameEntities::StagedChange *> *)data;

	const std::unordered_map<int, Player> &players = demoref->getPlayers();
	// for (auto i = players.begin(); i != players.end(); i++)
	// {
	// 	std::cout << i->first << " " << i->second.md.userName << std::endl;
	// }
	// exit(1);
	
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
			PrintExists("m_iHealth", int, "health");
			PrintExists("m_iAccount", int, "balance");
			PrintExists("m_bHasFelmet", int, "hasHelmet");
			PrintExists("localdata.m_vecOrigin", Vector2, "lVecOriginXY");
			PrintExists("csnonlocaldata.m_vecOrigin", Vector2, "nlVecOriginXY");

		}
		i++;
		if (i != players.end())
			std::cout << " }," << std::endl;
		else
			std::cout << " }" << std::endl;
	}
	std::cout << "]" << std::endl;

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
	// DataTable *data = (DataTable *)d;

	// for (size_t i = 0; i < data->services.size(); i++)
	// {
	// 	std::cout << data->services[i] << std::endl;
	// }
	// exit(-1);	
}

int main(int argc, char **argv, char **env)
{
	(void)env;

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
	auto	startTime = std::chrono::high_resolution_clock::now();

	DemoFile demo(f);
	demoref = &demo;

	auto	endTime = std::chrono::high_resolution_clock::now();
	auto	diffdTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	(void)diffdTime;
	// std::cout << "Preparser took" << diffdTime.count() / 1000 << "ms" << std::endl;

	if (startupParameters["--only-parse"] == 0)
	{
		std::cout << "[";
		// demo.addEventHook(svc_ServerInfo, printServerInfo);
		// demo.addEventHook(svc_GameEvent, printGameEvent);
		// demo.addEventHook(6, printDataTable);
		demo.addEventHook(svc_PacketEntities, printPacketEntities);
		// demo.addEventHook(svc_CreateStringTable, printCreateStringTable);

		demo.create_metrics();
		std::cout << "]" << std::endl;
	}
	google::protobuf::ShutdownProtobufLibrary();
	std::cerr << "Done!" << std::endl;
	return (0);
}