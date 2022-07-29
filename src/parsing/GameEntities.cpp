#include <demo.hpp>

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
	}

	std::vector< GameEntities::Property > topush;
	topush.reserve(numElem);

	toAdd.type = newProp.prop->type();
	toAdd.flags = newProp.prop->flags();
	toAdd.num_bits = newProp.prop->num_bits();
	toAdd.low_value = newProp.prop->low_value();
	toAdd.high_value = newProp.prop->high_value();

	for (int x = 0; x < numElem; x++)
	{
		switch (newProp.prop->type())
		{
			case 0:
				toAdd.data = decodeint(sr, toAdd);
				break;
			case 1:
				toAdd.data = decodefloat(sr, toAdd);
				break;
			case 2:
				toAdd.data = decodeVector(sr, toAdd);
				break;
			case 3:
				toAdd.data = decodeVector2(sr, toAdd);
				break;
			case 4:
				toAdd.data = decodestring(sr, toAdd);
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
		prop.second.data = decode##typeV(sr, prop.second);						\
		break;																	\
	}

	std::pair<const string *, GameEntities::Property> &prop = ent.properties[ind];
	if (!prop.first) [[unlikely]]
	{
		const PropW &flatProp = ent.parentService->props[ind];
	
		prop.first = &flatProp.path;
		prop.second.type = flatProp.prop->type();
		prop.second.flags = flatProp.prop->flags();
		prop.second.num_bits = flatProp.prop->num_bits();
		prop.second.low_value = flatProp.prop->low_value();
		prop.second.high_value = flatProp.prop->high_value();

	}

	switch (prop.second.type)
	{
		DecodeSwitch(0, int);
		DecodeSwitch(1, float);
		DecodeSwitch(2, Vector);
		DecodeSwitch(3, Vector2);
		DecodeSwitch(4, string);
		case 5:
		{
			DecodeArray(sr, ent.parentService->props[ind], ent, ind);
			break;
		}
	default:
		{
			assert (0);
		}
	}
#undef string
#undef DecodeSwitch
}

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
			data.emplace_back(++oldindex);
			data.emplace_back(++oldindex);
			break;
		}
		case 2: //10
		{
			data.emplace_back((oldindex += 1 + sr.readBits(3)));
			break;
		}
		case 1: //01
		{
			data.emplace_back(++oldindex);
			if (sr.readBit() != 0)
			{
				data.emplace_back((oldindex += 1 + sr.readBits(3)));
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
			data.emplace_back((oldindex += 1 + flags));
		}
		}

	}
}

std::vector<int> indicies;

void	readFromStream(StreamReader &sr, GameEntities::Entity &ent)
{
	bool readNewWay = sr.readBit();

	int index = -1;

	indicies.clear();
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

inline void	deleteFromIndex(std::unordered_multimap<std::string, int> &indexes, int &currentEntity, std::string &name)
{
	for (auto it = indexes.equal_range(name); it.first != it.second; it.first++)
	{
		if (it.first->second == currentEntity)
		{
			indexes.erase(it.first);
			break;
		}
	}
}

void GameEntities::parse(PacketEntities &pe, DataTable &dt, DemoFile &df)
{
	StreamReader sr(pe.entity_data());		
	int currentEntity = -1;

	for (int x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + sr.readStreamInt();

		if (currentEntity >= props.size())
			props.resize((currentEntity + 2) * 2);
		Entity	&toChange = props[currentEntity];

		int type = sr.readBits(2);

		switch (type)
		{
		case 0:		// update
			{
				readFromStream(sr, toChange);
				df.emitEvent(svc_UpdateEntity, &toChange);
				break;
			}
		case 2:		// create
			{

				if (toChange.parentService != 0)
				{
					deleteFromIndex(indexes, currentEntity, toChange.parentService->nameDataTable);
					toChange.properties.clear();
					toChange.properties.resize(toChange.properties.capacity());
				}
				toChange.parentService = PVSParser(sr, dt);
				if (toChange.properties.size() < toChange.parentService->props.size())
					toChange.properties.resize(toChange.parentService->props.size());
				readFromStream(sr, toChange);
				indexes.insert(std::make_pair(toChange.parentService->nameDataTable, currentEntity));
				if (toChange.parentService->nameDataTable == "DT_CSPlayer")
				{
					df.getPlayer(currentEntity - 1).packetRef = &(props[currentEntity]);
				}

				df.emitEvent(svc_CreateEntity, &toChange);
				break;
			}
		default:	// delete
			{
				df.emitEvent(svc_DeleteEntity, &toChange);

				if (toChange.parentService->nameDataTable == "DT_CSPlayer")
				{
					df.getPlayer(currentEntity - 1).packetRef = 0;
				}

				deleteFromIndex(indexes, currentEntity, toChange.parentService->nameDataTable);
				toChange.properties.clear();
				toChange.parentService = 0;
				break;
			}
		}
	}
}

GameEntities::GameEntities() {
	props.resize(512);
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
	data = s.data;

	type = s.type;
	flags  = s.flags;
	num_bits = s.num_bits;
	low_value = s.low_value;
	high_value = s.high_value;

	return (*this);
}

const GameEntities::Property *GameEntities::Entity::getProperty(std::string name) const
{
	if (!parentService)
		return 0;
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].first && name == *properties[i].first)
			return &properties[i].second;
	}
	return 0;
}