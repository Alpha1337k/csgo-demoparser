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
	int lastEntry = -1;
	StreamReader sr(data);
	
	(void)isUserInfo;

	// perform integer log2() to set nEntryBits
	int nTemp = origin.max_entries();
	int nEntryBits = 0;
	while (nTemp >>= 1) ++nEntryBits;


	bool isDictionary = sr.readBit() != 0;

	if (isDictionary)
		ErrorReturnMessage("Error: StringTable: encoded using dictionary")

	std::vector< std::string > history;

	for (int x = 0; x < num_changed_entries; x++)
	{
		int entryIndex = lastEntry + 1;

		if (sr.readBit() == 0)
			entryIndex = sr.readBits(nEntryBits);
		lastEntry = entryIndex;
		if (entryIndex < 0 || entryIndex >= origin.max_entries())
			ErrorReturnMessage("Error: ParseStringUpdate Error")

		std::string entry;
		if (sr.readBit() != 0)
		{
			int copySize = 1024;
			if (sr.readBit() != 0)
				sr.readBits(10);
			char charToAdd = ' ';
			int count = 0;
			while (charToAdd != 0 && charToAdd != '\n' && count < copySize)
			{
				charToAdd = sr.readBits(8);
				entry += charToAdd;
				count++;
			}
		}

		std::string userData;
		size_t size = 0;

		if (sr.readBit() != 0)
		{
			if (origin.user_data_fixed_size())
			{
				size = origin.user_data_size();
			}
			else
			{
				size = sr.readBits(14);
			}
			char charToAdd;
			while (size > userData.size())
			{
				charToAdd = sr.readBits(8);
				userData += charToAdd;
			}
		}
		if (size > 0)
		{
			assert(size < 400);
			Player p(userData);
			// std::cout << p << std::endl;
			assert(p.version == -4094);
			df.AddPlayer(p);
		}
		assert(!sr.isEof());
	}
}

void DemoFile::AddPlayer(Player &p)
{
	players.push_back(p);
}
