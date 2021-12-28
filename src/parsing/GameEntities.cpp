#include <demo.hpp>

void	readFieldIndex(StreamReader &sr, std::vector<int> &data)
{
	int oldindex = -1;
	while (1)
	{
		int flags = sr.readBits(2);
		switch (flags)
		{
		case 3: //11
		{
			data.push_back(++oldindex);
			data.push_back(++oldindex);
			break;
		}
		case 2: //10
		{
			data.push_back((oldindex += 1 + sr.readBits(3)));
			break;
		}
		case 1: //01
		{
			data.push_back(++oldindex);
			if (sr.readBit() != 0)
			{
				data.push_back((oldindex += 1 + sr.readBits(3)));
				break;
			}
		}
		default: [[unlikely]]
		{
			flags = sr.readBits(7);
			switch (flags & (32 | 64))
			{
				case 32:
					flags = (flags & ~96) | (sr.readBits(2) << 5);
					break;
				case 64:
					flags = (flags & ~96) | (sr.readBits(4) << 5);
					break;
				case 96:
					flags = (flags & ~96) | (sr.readBits(7) << 5);
					break;
				default:
					break;
			}
			if (flags == 0xFFF)
				return;
			data.push_back((oldindex += 1 + flags));
		}
		}

	}
}

size_t fulltime = 0;

void printdecTime()
{
	std::cout << ' ' << fulltime << "us" << '\n';
}

inline void	DecodeArray(StreamReader &sr, const PropW &flatProp, GameEntities::Entity &ent, int &ind)
{
	int maxElem = flatProp.prop->num_elements();

	int bitsToRead = 1;
	while (maxElem >>= 1)
		bitsToRead++;
	int numElem = sr.readBits(bitsToRead);

	PropW &newProp = ent.parentService->arProps[flatProp.targetElem];
	GameEntities::Property toAdd;
	GameEntities::Property &prop = ent.properties[ind].second;

	if (ent.properties[ind].first != &newProp.path)
	{
		ent.properties[ind].first = &newProp.path;
		prop.type = decoded_array;
	}

	std::vector< GameEntities::Property > topush;
	topush.reserve(numElem);

	toAdd.type = newProp.prop->type();
	for (int x = 0; x < numElem; x++)
	{
		switch (newProp.prop->type())
		{
			case 0:
				toAdd.data = decodeint(sr, *newProp.prop);
				break;
			case 1:
				toAdd.data = decodefloat(sr, *newProp.prop);
				break;
			case 2:
				toAdd.data = decodeVector(sr, *newProp.prop);
				break;
			case 3:
				toAdd.data = decodeVector2(sr, *newProp.prop);
				break;
			case 4:
				toAdd.data = decodestring(sr, *newProp.prop);
				break;
			default:
				assert(0);
				break;
		}
		topush.push_back(toAdd);
	}
	prop.data = topush;
}

void	decodeProperty(StreamReader &sr, int &ind, GameEntities::Entity &ent)
{
using std::string;
#define DecodeSwitch(i, typeV)													\
	case i:																		\
	{																			\
		prop.second.data = decode##typeV(sr, *flatProp.prop);					\
		break;																	\
	}
	assert(ind < (int)ent.parentService->props.size());
	assert(ind < ent.properties.size());

	const PropW &flatProp = ent.parentService->props[ind];

	std::pair<const string *, GameEntities::Property> &prop = ent.properties[ind];
	if (!prop.first) [[unlikely]]
	{
		prop.first = &flatProp.path;
		prop.second.type = flatProp.prop->type();
	}

	switch (flatProp.prop->type())
	{
		DecodeSwitch(0, int);
		DecodeSwitch(1, float);
		DecodeSwitch(2, Vector);
		DecodeSwitch(3, Vector2);
		DecodeSwitch(4, string);
		case 5:
		{
			DecodeArray(sr, flatProp, ent, ind);
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

void	readFromStream(StreamReader &sr, GameEntities::Entity &ent)
{
	bool readNewWay = sr.readBit();

	std::vector<int> indicies;
	int index = -1;

	indicies.reserve(20);
	readFieldIndex(sr, indicies);

	for (size_t x = 0; x < indicies.size(); x++)
	{
		decodeProperty(sr, indicies[x], ent);
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
				readFromStream(sr, toChange);
				break;
			}
		case 2:		// create
			{
				toChange.parentService = PVSParser(sr, dt);
				toChange.properties.resize(toChange.parentService->props.size());
				readFromStream(sr, toChange);
				indexes.insert(std::make_pair(toChange.parentService->nameDataTable, currentEntity));
				if (toChange.parentService->nameDataTable == "DT_CSPlayer")
				{
					df.getPlayer(currentEntity - 1).packetRef = &(props[currentEntity]);
				}
				break;
			}
		default:
			{
				auto it = indexes.equal_range(toChange.parentService->nameDataTable);
				for (; it.first != it.second; it.first++)
				{
					if (it.first->second == currentEntity)
					{
						indexes.erase(it.first);
						break;
					}
				}
				toChange.properties.clear();
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