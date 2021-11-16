#include <demo.hpp>

ParsedStringTable::ParsedStringTable(CreateStringTable &st): origin(st)
{
	if (st.name() == "userinfo")
	{
		Update(st.string_data(), st.name() == "userinfo", st.num_entries());
		exit(9);
	}
}


ParsedStringTable::~ParsedStringTable() {}

void	ParsedStringTable::Update(CreateStringTable &st)
{
	Update(st.string_data(), st.name() == "userinfo", st.num_entries());
}

void	ParsedStringTable::Update(UpdateStringTable &ut, bool isUserInfo)
{
	Update(ut.string_data(), isUserInfo, ut.num_changed_entries());
}

void	ParsedStringTable::Update(const std::string &data, bool isUserInfo, int num_changed_entries)
{
#define readBits(x) readStringBits(data, x, i, bitsAvailable)

	int i = 0;
	int lastEntry = -1;
	int lastDictionaryIndex = -1;
	char bitsAvailable = 0;



	// perform integer log2() to set nEntryBits
	int nTemp = origin.max_entries();
	int nEntryBits = 0;
	while (nTemp >>= 1) ++nEntryBits;

	bool isDictionary = readBits(1) != 0;

	if (isDictionary)
		ErrorReturnMessage("Error: StringTable: encoded using dictionary")

	std::vector< char[1 << 5] > history;
	std::cout << data << std::endl;

	for (size_t x = 0; x < num_changed_entries; x++)
	{
		int entryIndex = lastEntry + 1;

		if (readBits(1) == 0)
		{
			entryIndex = readBits(nEntryBits);
			std::cout << "new index: " << entryIndex << std::endl;
		}
			
		lastEntry = entryIndex;
		if (entryIndex < 0 || entryIndex >= origin.max_entries())
			ErrorReturnMessage("Error: ParseStringUpdate Error")

		std::string entry;
		if (readBits(1) != 0)
		{
			if (readBits(1) != 0)
			{
				int index = readBits(5);
				int copySize = readBits(5);
			}
			entry = readVarString(data, i);
			std::cout << "Entry: " << entry.length() << std::endl;
		}

		std::string userData;
		int size = 0;

		if (readBits(1) != 0)
		{
			if (origin.user_data_fixed_size())
			{
				size = origin.user_data_size();
				std::cout << "Using fixed!!!!" << std::endl;
			}
			else
			{
				int bytesize = readBits(14);
				size = bytesize;
			}
			std::cout << "We have a size: " << size <<  std::endl;
			userData = data.substr(i, size);
			i += size;
		}
		if (size > 0)
		{
			Player p = *(Player *)&userData[0];
			std::cout << "Name: " << p.version << std::endl;
		}
	}
#undef readBits
}
