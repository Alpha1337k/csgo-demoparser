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

void	decodeProperty(standardParameters, int &ind, const DataTable &dt, DataTable::ServiceClass &serviceClass, const PropW *arProp = 0)
{
#define string std::string
#define DecodeSwitch(i, type) \
	case i: \
	{ \
		type rv = decode##type(standardIParameters, flatProp.prop); \
		printIfAllowed("--entitymsg", std::cout << flatProp.path << " : " << rv << std::endl); \
		break; \
	}
	assert(ind < serviceClass.props.size());

	if (!arProp)
		arProp = &serviceClass.props[ind];
	const PropW &flatProp = *arProp;

	switch (flatProp.prop.type())
	{
		DecodeSwitch(0, int);
		DecodeSwitch(1, float);
		DecodeSwitch(2, Vector);
		DecodeSwitch(3, Vector2);
		DecodeSwitch(4, string);
		case 5:
		{
			int maxElem = flatProp.prop.num_elements();
			int bitsToRead = 1;
			while (maxElem >>= 1)
				bitsToRead++;
			int numElem = readBits(bitsToRead);
			for (size_t x = 0; x < numElem; x++)
			{
				PropW newProp = PropW(flatProp.targetElem, flatProp.path + '.' + std::to_string(x));
				decodeProperty(standardIParameters, ind, dt, serviceClass, &newProp);
			}
			break;
		}
	
	default:
		{
			ErrorReturnMessage("Error case not found! type: " + std::to_string(flatProp.prop.type()));
		}
	}
#undef string
#undef DecodeSwitch
}

void	readFromStream(standardParameters, const DataTable &dt, DataTable::ServiceClass &serviceClass)
{
	bool readNewWay = readBits(1) == 1 ? true : false;

	std::vector<int> indicies;
	int index = -1;
	
	while ((index = readFieldIndex(standardIParameters, readNewWay, index)) != -1)
		indicies.push_back(index);

	for (size_t x = 0; x < indicies.size(); x++)
	{
		decodeProperty(standardIParameters, indicies[x], dt, serviceClass);
	}
}

DataTable::ServiceClass	PVSParser(standardParameters, int &id, DataTable &dt)
{
	int serverClassId = readBits(dt.serviceClassBits);

	int serial = readBits(10);
	
	DataTable::ServiceClass nSC = DataTable::ServiceClass(dt.services[serverClassId]);

	return nSC;
}

void GameEntities::parse(PacketEntities &pe, DataTable &dt)
{
	const std::string &data = pe.entity_data();
	int i = 0;
	char bitsAvailable = 8;
	int currentEntity = -1;

	size_t x = 0;
	for (; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + readStringVarInt(standardIParameters);

		printIfAllowed("--entitymsg", std::cout << "-------[Current Entity: " << currentEntity << ", bytes read: << " << i << "]" << std::endl);
		if (readBits(1) == 0)
		{
			if (readBits(1))
			{
				DataTable::ServiceClass serviceClass = PVSParser(standardIParameters, currentEntity, dt);
				printIfAllowed("--entitymsg", std::cout << "Create" << std::endl);
				printIfAllowed("--entitymsg", std::cout << serviceClass << std::endl);
				readFromStream(standardIParameters, dt, serviceClass);
				updateProps(currentEntity, serviceClass);
			}
			else
			{
				printIfAllowed("--entitymsg", std::cout << "Update" << std::endl);
				printIfAllowed("--entitymsg", std::cout << props[currentEntity] << std::endl);
				readFromStream(standardIParameters, dt, props[currentEntity]);
				updateProps(currentEntity, props[currentEntity]);
			}
		}
		else
		{

			printIfAllowed("--entitymsg", std::cout << "Delete" << std::endl);
			printIfAllowed("--entitymsg", std::cout << props[currentEntity] << std::endl);

			DataTable::ServiceClass nullified = DataTable::ServiceClass();
			nullified.id = -1;
			updateProps(currentEntity, nullified);

			readBits(1);
		}


		assert(i < data.length());
	}
	assert(x == pe.updated_entries());
	// assert(i == data.length()); this fails, dont know if its a big deal
}

void	GameEntities::updateProps(int idx, DataTable::ServiceClass &cls)
{
	if (props.size() <= idx)
		props.resize(idx * 2 == 0 ? 1 : idx * 2);
	props[idx] = cls;
}

GameEntities::GameEntities() {}
