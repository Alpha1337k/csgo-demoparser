#include <demo.hpp>

#define readBits(x) readStringBits(data, x, i, bitsAvailable)

int		readFieldIndex(const std::string &data, int &i, char &bitsAvailable, bool newWay, int oldindex)
{
	if (newWay && readBits(1))
	{
		return oldindex + 1;
	}
	int rval = 0;
	if (newWay && readBits(1))
		rval = readBits(3);
	else
	{
		rval = readBits(7);
		switch (rval & (32 | 64))
		{
			case 32:
				rval = (rval & ~96) | (readBits(2) << 5);
				break;
			case 64:
				rval = (rval & ~96) | (readBits(4) << 5);
				break;
			case 96:
				rval = (rval & ~96) | (readBits(7) << 5);
				break;
			default:
				break;
		}
	}
	if (rval == 0xFFF)
		return -1;
	return oldindex + 1 + rval;
}

void	readFromStream(const std::string &data, int &i, char &bitsAvailable)
{
	bool readNewWay = readBits(1);
	std::vector<int> indicies;
	int index = 0;

	std::cout << "new way: " << readNewWay << std::endl;
	while ((index = readFieldIndex(data, i, bitsAvailable, readNewWay, index)) != -1)
		indicies.push_back(index);

	for (size_t i = 0; i < indicies.size(); i++)
	{
		std::cout << "I: " << indicies[i] << std::endl;
	}
	
	
}

DataTable::ServiceClass	PVSParser(const std::string &data, int &i, char &bitsAvailable, int &id, const DataTable &dt)
{
	int serverClassId = readBits(5);

	std::cout << "serverClassId: " << serverClassId << std::endl;

	readBits(10);
	
	DataTable::ServiceClass nSC = DataTable::ServiceClass(dt.services[serverClassId]);

	std::cout << "New Entity: { id: " << nSC.id << ", name: " << nSC.name \
		<< ", tableName: " << nSC.nameDataTable << "}" << std::endl;

	return nSC;
}

ParsedPacketEntities::ParsedPacketEntities(PacketEntities &pe, const DataTable &dt)
{
	const std::string &data = pe.entity_data();
	int i = 0;
	char bitsAvailable = 8;
	int currentEntity = -1;

	for (size_t x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + readStringVarInt(data, i, bitsAvailable);

		std::cout << "CurrentEn: " << currentEntity << std::endl;
		if (readBits(1) == 0)
		{
			// create
			if (readBits(1))
			{
				std::cerr << "Create" << std::endl;
				DataTable::ServiceClass serviceClass = PVSParser(data, i, bitsAvailable, currentEntity, dt);
				readFromStream(data, i, bitsAvailable);
				exit(1);
			}
			// update
			else
			{
				std::cerr << "Update" << std::endl;
				
				exit(1);
			}
		}
		// delete
		else
		{
			std::cerr << "Delete" << std::endl;
			readBits(1);
			exit(1);
		}


		assert(i < data.length());
	}

}

#undef readBits