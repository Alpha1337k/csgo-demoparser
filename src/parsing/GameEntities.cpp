#include <demo.hpp>

int		readFieldIndex(StreamReader &sr, bool newWay, int oldindex)
{
	int rval;
	if (newWay)
	{
		if (sr.readBit())
			return oldindex + 1;
		else if (sr.readBit())
			return oldindex + 1 + sr.readBits(3);
	}
	rval = sr.readBits(7);
	switch (rval & (32 | 64))
	{
		case 32:
			rval = (rval & ~96) | (sr.readBits(2) << 5);
			break;
		case 64:
			rval = (rval & ~96) | (sr.readBits(4) << 5);
			break;
		case 96:
			rval = (rval & ~96) | (sr.readBits(7) << 5);
			break;
		default:
			break;
	}
	if (rval == 0xFFF)
		return -1;
	return oldindex + 1 + rval;
}

size_t fulltime = 0;

void printdecTime()
{
	std::cout << ' ' << fulltime << "us" << '\n';
}

void	decodeProperty(StreamReader &sr, int &ind, const DataTable &dt, GameEntities::Entity &ent, const PropW *arProp = 0)
{
using std::string;
#define DecodeSwitch(i, typeV)													\
	case i:																		\
	{																			\
		std::pair<const string *, GameEntities::Property> &prop = ent.properties[ind];	\
		prop.first = &flatProp.path;											\
		prop.second.type = decoded_##typeV;										\
		prop.second.data = decode##typeV(sr, *flatProp.prop);					\
		break;																	\
	}
	assert(ind < (int)ent.parentService->props.size());
	assert(ind < ent.properties.size());

	if (!arProp)
		arProp = &ent.parentService->props[ind];
	const PropW &flatProp = *arProp;

	switch (flatProp.prop->type())
	{
		DecodeSwitch(0, int);
		DecodeSwitch(1, float);
		DecodeSwitch(2, Vector);
		DecodeSwitch(3, Vector2);
		DecodeSwitch(4, string);
		case 5:
		{
			int maxElem = flatProp.prop->num_elements();
			int bitsToRead = 1;
			while (maxElem >>= 1)
				bitsToRead++;
			int numElem = sr.readBits(bitsToRead);
			PropW newProp = PropW(flatProp.targetElem, "");
			GameEntities::Property toAdd;
			GameEntities::Property &prop = ent.properties[ind].second;

			ent.properties[ind].first = &newProp.path;
			std::vector< GameEntities::Property > topush;
			topush.reserve(numElem);
			for (int x = 0; x < numElem; x++)
			{
				switch (newProp.prop->type())
				{
				case 0:
					toAdd.type = decoded_int;
					toAdd.data = decodeint(sr, *newProp.prop);
					break;
				case 1:
					toAdd.type = decoded_float;
					toAdd.data = decodefloat(sr, *newProp.prop);
					break;
				case 2:
					toAdd.type = decoded_Vector;
					toAdd.data = decodeVector(sr, *newProp.prop);
					break;
				case 3:
					toAdd.type = decoded_Vector2;
					toAdd.data = decodeVector2(sr, *newProp.prop);
					break;
				case 4:
					toAdd.type = decoded_string;
					toAdd.data = decodestring(sr, *newProp.prop);
					break;
				default:
					assert(0);
					break;
				}
				topush.push_back(toAdd);
			}
			prop.type = decoded_array;
			prop.data = topush;
			break;
		}
	
	default:
		{
			ErrorReturnMessage("Error case not found! type: " + std::to_string(flatProp.prop->type()));
		}
	}
#undef string
#undef DecodeSwitch
}

void	readFromStream(StreamReader &sr, const DataTable &dt, GameEntities::Entity &ent)
{
	bool readNewWay = sr.readBit();

	std::vector<int> indicies;
	int index = -1;

	indicies.reserve(20);
	while ((index = readFieldIndex(sr, readNewWay, index)) != -1)
		indicies.push_back(index);

	for (size_t x = 0; x < indicies.size(); x++)
	{
		decodeProperty(sr, indicies[x], dt, ent);
	}

}

DataTable::ServiceClass	*PVSParser(StreamReader &sr, DataTable &dt)
{
	int serverClassId = sr.readBits(dt.serviceClassBits);

	sr.readBits(10);

	assert(serverClassId < dt.services.size());

	return &dt.services[serverClassId];
}

size_t highest = 0;

void GameEntities::parse(PacketEntities &pe, DataTable &dt, DemoFile &df)
{
	StreamReader sr(pe.entity_data());		
	int currentEntity = -1;

	for (int x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + sr.readStreamInt();

		assert(currentEntity < 700); // error for static vector size. please report if spotted in the wild
		Entity	&toChange = props[currentEntity];

		int type = sr.readBits(2);

		switch (type)
		{
		case 0:		// update
			{
				readFromStream(sr, dt, toChange);
				break;
			}
		case 2:		// create
			{
				toChange.parentService = PVSParser(sr, dt);
				toChange.properties.resize(toChange.parentService->props.size());
				readFromStream(sr, dt, toChange);
				if (toChange.parentService->nameDataTable == "DT_CSPlayer")
				{
					df.getPlayer(currentEntity - 1).packetRef = &(props[currentEntity]);
				}
				break;
			}
		default:
			{
				toChange.parentService = 0;
				break;
			}
		}
	}
}

GameEntities::GameEntities() {
	props.resize(700);
}

GameEntities::Property::~Property()
{
}

GameEntities::Entity		&GameEntities::Entity::operator=(const GameEntities::Entity &s)
{
	parentService = s.parentService;
	properties = s.properties;

	return (*this);
}

GameEntities::Property		&GameEntities::Property::operator=(const GameEntities::Property &s)
{
	type = s.type;
	data = s.data;

	return (*this);
}