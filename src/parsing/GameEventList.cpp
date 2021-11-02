#include <demo.hpp>

GameEventListParsed::GameEventListParsed(GameEventList &l)
{
	for (size_t i = 0; i < l.descriptors_size(); i++)
	{
		const GameEventList_descriptor_t &dsc = l.descriptors(i);
		std::cout << "{" << std::endl;
		std::cout << "id:" << dsc.eventid() << std::endl;
		std::cout << "name:" << dsc.name() << std::endl;
		for (size_t x = 0; x < dsc.keys_size(); x++)
		{
			const GameEventList_key_t &key = dsc.keys(x);
			std::cout << "keys: {" << std::endl;
			std::cout << "	type:" << key.type() << std::endl;
			std::cout << "	name:" <<  key.name() << std::endl;
			std::cout << "}" << std::endl;

		}
		
		std::cout << "}" << std::endl;
		
	}
	exit(0);
}