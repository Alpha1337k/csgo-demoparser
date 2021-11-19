#include <demo.hpp>

ParsedStringTable::ParsedStringTable(CreateStringTable &st, DemoFile &df): origin(st)
{
	if (st.name() == "userinfo")
	{
		Update(st.string_data(), st.name() == "userinfo", st.num_entries(), df);
	}
}


ParsedStringTable::~ParsedStringTable() {}

void	ParsedStringTable::Update(CreateStringTable &st, DemoFile &df)
{
	Update(st.string_data(), st.name() == "userinfo", st.num_entries(), df);
}

void	ParsedStringTable::Update(UpdateStringTable &ut, DemoFile &df, bool isUserInfo)
{
	Update(ut.string_data(), isUserInfo, ut.num_changed_entries(), df);
}

void	ParsedStringTable::Update(const std::string &data, bool isUserInfo, int num_changed_entries, DemoFile &df)
{
#define readBits(x) readStringBits(data, x, i, bitsAvailable)

	int i = 0;
	int lastEntry = -1;
	int lastDictionaryIndex = -1;
	char bitsAvailable = 8;

	// perform integer log2() to set nEntryBits
	int nTemp = origin.max_entries();
	int nEntryBits = 0;
	while (nTemp >>= 1) ++nEntryBits;


	bool isDictionary = readBits(1) != 0;

	if (isDictionary)
		ErrorReturnMessage("Error: StringTable: encoded using dictionary")

	std::vector< std::string > history;

	for (size_t x = 0; x < num_changed_entries; x++)
	{
		//std::cout << "Start------" << std::endl;
		int entryIndex = lastEntry + 1;

		if (readBits(1) == 0)
		{
			entryIndex = readBits(nEntryBits);
			//std::cout << "new index: " << entryIndex << std::endl;
		}
		//std::cout << "Entry index: " << entryIndex << std::endl;
		lastEntry = entryIndex;
		if (entryIndex < 0 || entryIndex >= origin.max_entries())
			ErrorReturnMessage("Error: ParseStringUpdate Error")

		std::string entry;
		if (readBits(1) != 0)
		{
			int copySize = 1024;
			if (readBits(1) != 0)
			{
				int index = readBits(5);
				int copyHistSize = readBits(5);
				//std::cout << "ReadBits Called! Whoops" << index << " " << copyHistSize << std::endl;
			}
			char charToAdd = ' ';
			int count = 0;
			while (charToAdd != 0 && charToAdd != '\n' && count < copySize)
			{
				charToAdd = readBits(8);
				entry += charToAdd;
				count++;
			}
			//std::cout << "Entry size: " << entry.length() << " "<< entry << std::endl;
		}

		std::string userData;
		int size = 0;

		if (readBits(1) != 0)
		{
			if (origin.user_data_fixed_size())
			{
				size = origin.user_data_size();
				//std::cout << "Using fixed!!!!" << std::endl;
			}
			else
			{
				size = readBits(14);
			}
			//std::cout << "We have a size: " << size <<  std::endl;
			char charToAdd;
			while (size > userData.size())
			{
				charToAdd = readBits(8);
				userData += charToAdd;
			}
			//std::cout << "Userdata size: " << userData.size() << "{ " << userData << "}" << std::endl;
		}
		if (size > 0)
		{
			assert(size < 400);
			Player p(userData);
			// std::cout << p << std::endl;
			assert(p.version == -4094);
			df.AddPlayer(p);
		}
		assert(i < data.length());
	}
#undef readBits
}

void DemoFile::AddPlayer(Player &p)
{
	players.push_back(p);
}
