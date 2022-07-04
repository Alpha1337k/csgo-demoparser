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

// gets the updated packet
// *d is an constant pointer, till deleted or end.
// can read all the properties that are loaded in.
// data->parentService->props holds all the possible props, at the same index as properties.
void	updatePacket(void *d)
{
	GameEntities::Entity *data = (GameEntities::Entity *)d;

	
	std::cout << "Update: " << data->parentService->name << std::endl;
	if (data->parentService->name == "CWeaponM4A1") {
		for (size_t i = 0; i < data->properties.size(); i++)
		{
			if (!data->properties[i].first)
				continue;
			std::cout << *data->properties[i].first << std::endl;
			std::cout << data->properties[i].second.type << std::endl;
		}
	}
}

void	createPacket(void *d)
{
	GameEntities::Entity *data = (GameEntities::Entity *)d;

	std::cout << "Create: " << data->parentService->name << std::endl;
}

void	deletePacket(void *d)
{
	GameEntities::Entity *data = (GameEntities::Entity *)d;

	std::cout << "Delete: " << data->parentService->name << std::endl;
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

	// std::function<void(void *)> eventFP = &printPacketEntities;
	// std::function<void(void *)> eventDelete = &deletePacket;
	// std::function<void(void *)> eventCreate = &createPacket;
	// std::function<void(void *)> eventUpdate = &updatePacket;
	// demo.addEventHook(svc_PacketEntities, eventFP);
	// demo.addEventHook(svc_DeleteEntity, eventDelete);
	// demo.addEventHook(svc_CreateEntity, eventCreate);
	// demo.addEventHook(svc_UpdateEntity, eventUpdate);


	// std::cout << "[";

	demo.start_parse(f);
	// std::cout << "]" << std::endl;
	auto	parseTime = std::chrono::high_resolution_clock::now();
	printTime("Parsing", parseTime, startEnd);

	google::protobuf::ShutdownProtobufLibrary();
	std::cerr << "Done!" << std::endl;
	return (0);
}