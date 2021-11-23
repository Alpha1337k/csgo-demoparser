#include <demo.hpp>

#define readBits(x) readStringBits(data, x, i, bitsAvailable)
#define standardParameters const std::string &data, int &i, char &bitsAvailable

int		readFieldIndex(standardParameters, bool newWay, int oldindex)
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

void	readFromStream(standardParameters)
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

DataTable::ServiceClass	PVSParser(standardParameters, int &id, const DataTable &dt)
{
	int serverClassId = readBits(dt.serviceClassBits);

	std::cout << "serverClassId: " << serverClassId << ", Bits: " << (int)dt.serviceClassBits << std::endl;

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

	std::cout << "updated: " << pe.updated_entries() << std::endl;
	for (size_t x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + readStringVarInt(data, i, bitsAvailable);

		std::cout << "CurrentEn: " << currentEntity << std::endl;
		if (readBits(1) == 0)
		{
			// create
			if (readBits(1))
			{
				std::cout << "Create" << std::endl;
				DataTable::ServiceClass serviceClass = PVSParser(data, i, bitsAvailable, currentEntity, dt);
				readFromStream(data, i, bitsAvailable);
				//exit(0);
			}
			// update
			else
			{
				std::cout << "Update" << std::endl;
				
			}
		}
		// delete
		else
		{
			std::cout << "Delete" << std::endl;
			readBits(1);
			exit(1);
		}


		assert(i < data.length());
	}

}

#undef readBits