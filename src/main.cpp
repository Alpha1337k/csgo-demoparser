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
	std::vector<GameEntities::StagedChange *> *v = (std::vector<GameEntities::StagedChange *> *)data;

	std::cout << "ParsedPacket: " << v->size() << std::endl;
	// // for (size_t i = 0; i < ent.size(); i++)
	// // {
	// // 	std::cout << "StagedChange: { type: " << (int)ent[i].type << ", index: " << ent[i].index << ", data: {" << std::endl;
	// // 	std::cout << '\t' << ent[i].data.parentService << std::endl;
	// // 	for (size_t x = 0; x < ent[i].data.properties.size(); x++)
	// // 	{
	// // 		switch (ent[i].data.properties[x].type)
	// // 		{
	// // 		case decoded_int:
	// // 			std::cout << '\t' << ent[i].data.properties[x].name << " : " << *(int *)ent[i].data.properties[x].data << std::endl;
	// // 			break;
	// // 		case decoded_float:
	// // 			std::cout << '\t' << ent[i].data.properties[x].name << " : " << *(float *)ent[i].data.properties[x].data << std::endl;
	// // 			break;
	// // 		case decoded_Vector:
	// // 			std::cout << '\t' << ent[i].data.properties[x].name << " : " << *(Vector *)ent[i].data.properties[x].data << std::endl;
	// // 			break;
	// // 		case decoded_Vector2:
	// // 			std::cout << '\t' << ent[i].data.properties[x].name << " : " << *(Vector2 *)ent[i].data.properties[x].data << std::endl;
	// // 			break;
	// // 		case decoded_string:
	// // 			std::cout << '\t' << ent[i].data.properties[x].name << " : " << *(std::string *)ent[i].data.properties[x].data << std::endl;
	// // 			break;
	// // 		default:
	// // 			break;
	// // 		}
	// // 	}
	// // 	std::cout << "}\n";
		
	// // }
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

	if (f.load(startupParameters.last().first.c_str()) == -1)
	{
		std::cerr << "Error: file could not be opened" << std::endl;
		return (-1);
	}

	DemoFile demo(f);
	demoref = &demo;

	if (startupParameters["--only-parse"] == 0)
	{
		// demo.addEventHook(svc_ServerInfo, printServerInfo);
		// demo.addEventHook(svc_GameEvent, printGameEvent);
		// demo.addEventHook(svc_PacketEntities, printPacketEntities);


		demo.create_metrics();
	}
	google::protobuf::ShutdownProtobufLibrary();
	std::cerr << "Done!" << std::endl;
	return (0);
}