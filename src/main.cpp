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

	static int c = 0;
	std::vector<GameEntities::StagedChange *> *v = (std::vector<GameEntities::StagedChange *> *)data;

	Player &plyr = demoref->getPlayer(1);

	// std::cout << plyr << std::endl;
	if (plyr.packetRef != 0)
	{
		std::map<std::string, GameEntities::Property>::iterator found = plyr.packetRef->properties.find("m_iHealth");

		if (found != plyr.packetRef->properties.end())
		{
			int seconds = (int)(demoref->getCurrentTick() * 0.0315) % 60;
			std::cout << (int)((demoref->getCurrentTick() * 0.0315) / 60)  << ":" << seconds << "=" << *(int *)found->second.data << std::endl;
		}
	}
}

void	printCreateStringTable(void *data)
{
	CreateStringTable *st = (CreateStringTable *)data;

	if (st->name() != "userinfo")
		return;

	const std::vector<Player> &players = demoref->getPlayers();
	for (size_t i = 0; i < players.size(); i++)
	{
		std::cout << players[i] << std::endl;
	}
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

	std::cout << "Preparser took" << diffdTime.count() / 1000 << "ms" << std::endl;

	if (startupParameters["--only-parse"] == 0)
	{
		// demo.addEventHook(svc_ServerInfo, printServerInfo);
		// demo.addEventHook(svc_GameEvent, printGameEvent);
		// demo.addEventHook(6, printDataTable);
		demo.addEventHook(svc_PacketEntities, printPacketEntities);
		// demo.addEventHook(svc_CreateStringTable, printCreateStringTable);

		demo.create_metrics();
	}
	google::protobuf::ShutdownProtobufLibrary();
	std::cerr << "Done!" << std::endl;
	return (0);
}