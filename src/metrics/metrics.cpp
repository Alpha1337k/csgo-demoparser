#include <demo.hpp>

void	DemoFile::handleGameEventList(GameEventList &ge)
{
	gEvents.reserve(ge.descriptors_size());
	for (size_t i = 0; i < ge.descriptors_size(); i++)
	{
		const GameEventList_descriptor_t &dsc = ge.descriptors(i);
		gEvents.push_back(dsc);
	}
}

void	DemoFile::handleGameEvent(GameEvent &ge)
{
#define SwitchPrint(val, toprint) \
	case val: \
	{			\
		std::cout << eventKey.toprint(); \
		break;	\
	}
	if (ge.eventid() >= gEvents.size())
		throw std::overflow_error("Eventid is more than gEvents");
	const GameEventList_descriptor_t &event = gEvents[ge.eventid()];
	std::cout << "Event " << event.name() << ":{ ";
	for (size_t i = 0; i < event.keys_size(); i++)
	{
		const GameEventList_key_t &key = event.keys(i);
		const GameEvent_key_t &eventKey = ge.keys(i);
		std::cout << key.name() << ": ";
		eventKey.val_string();
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
		std::cout << ", ";
	}
	std::cout << "}\n";
#undef SwitchPrint
}

void	DemoFile::create_metrics()
{
#define HandleCase(value, type) \
	case value: \
	{			\
		handle##type(*(type *)pd.second); \
		break;	\
	}			\

	for (size_t i = 0; i < frames.size(); i++)
	{
		if (frames[i].cmd == dem_signon || frames[i].cmd == dem_packet)
		{
			for (size_t x = 0; x < frames[i].pckt.msg.size(); x++)
			{
				const std::pair<PacketTypes, void *> &pd = frames[i].pckt.msg[x];

				switch (pd.first)
				{
					HandleCase(MSG_GAME_EVENTS_LIST, GameEventList);
					HandleCase(MSG_GAME_EVENT, GameEvent);

				
				default:
					break;
				}
			}

		}
	}
#undef HandleCase
}