#include <demo.hpp>

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

void	decodeProperty(standardParameters, int &ind, const DataTable &dt, DataTable::ServiceClass &serviceClass)
{
#define string std::string
#define DecodeSwitch(i, type) \
	case i: \
	{ \
		type rv = decode##type(standardIParameters, flatProp.prop); \
		std::cout << "rv: " << rv << std::endl; \
		break; \
	}

	std::cout << "Propcount: " << serviceClass.props.size() << std::endl;
	if (ind > serviceClass.props.size())
		return;
	const PropW &flatProp = serviceClass.props[ind];

	std::cout << flatProp.prop.var_name() << ", " << flatProp.prop.type() << std::endl;
	switch (flatProp.prop.type())
	{
		DecodeSwitch(0, int);
		DecodeSwitch(1, float);
		DecodeSwitch(2, Vector);
		DecodeSwitch(3, Vector2);
		DecodeSwitch(4, string);
	
	default:
		{
			ErrorReturnMessage("Error case not found!");
		}
	}
#undef string
#undef DecodeSwitch
}

void	readFromStream(standardParameters, const DataTable &dt, DataTable::ServiceClass &serviceClass)
{
	bool readNewWay = readBits(1);
	std::vector<int> indicies;
	int index = 0;

	// std::cout << "new way: " << readNewWay << std::endl;
	while ((index = readFieldIndex(standardIParameters, readNewWay, index)) != -1)
		indicies.push_back(index);

	for (size_t x = 0; x < indicies.size(); x++)
	{
		std::cout << "indicie: " << indicies[x] << std::endl;
		decodeProperty(standardIParameters, indicies[i], dt, serviceClass);
	}
}

DataTable::ServiceClass	PVSParser(standardParameters, int &id, const DataTable &dt)
{
	int serverClassId = readBits(dt.serviceClassBits);

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

	std::cout << "-------------\nupdated entities: " << pe.updated_entries() << std::endl;
	for (size_t x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + readStringVarInt(standardIParameters);

		std::cout << "CurrentEn: " << currentEntity << std::endl;
		if (readBits(1) == 0)
		{
			// create
			if (readBits(1))
			{
				std::cout << "Create" << std::endl;
				DataTable::ServiceClass serviceClass = PVSParser(standardIParameters, currentEntity, dt);
				readFromStream(standardIParameters, dt, serviceClass);
				//exit(0);
			}
			// update
			else
			{
				std::cout << "Update" << std::endl;
				exit(1);
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
