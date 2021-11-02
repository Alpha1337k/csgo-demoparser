#include <demo.hpp>

GameEventParsed::GameEventParsed(GameEvent &l)
{
	std::cout << "{" << std::endl;
	std::cout << "id:" << l.eventid() << std::endl;
	std::cout << "name:" << l.event_name() << std::endl;
	for (size_t x = 0; x < l.keys_size(); x++)
	{
		const GameEvent_key_t &key = l.keys(x);
		std::cout << "keys: {" << std::endl;
		std::cout << "	" << key.DebugString() << std::endl;
		// switch (key.type())
		// {
		// 	case 1:
		// 		std::cout << key.val_string() << std::cout;
		// }
		std::cout << "	}," << std::endl;

	}

	std::cout << "}" << std::endl;

}