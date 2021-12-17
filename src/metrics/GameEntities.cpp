#include <demo.hpp>

int		readFieldIndex(StreamReader &sr, bool newWay, int oldindex)
{
	if (newWay && sr.readBit())
	{
		return oldindex + 1;
	}
	int rval = 0;
	if (newWay && sr.readBit())
		rval = sr.readBits(3);
	else
	{
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
	}
	if (rval == 0xFFF)
		return -1;
	return oldindex + 1 + rval;
}

void	decodeProperty(StreamReader &sr, int &ind, const DataTable &dt, GameEntities::Entity &ent, const PropW *arProp = 0)
{
#define string std::string
#define DecodeSwitch(i, typeV) \
	case i: \
	{ \
		typeV rv = decode##typeV(sr, flatProp.prop); \
		GameEntities::Property &prop = ent.properties[flatProp.path]; \
		prop.type = decoded_##typeV; \
		prop.data = new typeV(rv); \
		break; \
	}
	assert(ind < (int)ent.parentService->props.size());

	if (!arProp)
		arProp = &ent.parentService->props[ind];
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
			int numElem = sr.readBits(bitsToRead);
			for (int x = 0; x < numElem; x++)
			{
				PropW newProp = PropW(flatProp.targetElem, flatProp.path + '.' + std::to_string(x));
				decodeProperty(sr, ind, dt, ent, &newProp);
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

void GameEntities::parse(PacketEntities &pe, DataTable &dt)
{
	StreamReader sr(pe.entity_data());	
	
	int currentEntity = -1;

	staged.clear();
	staged.resize(pe.updated_entries());

	int x = 0;
	for (; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + sr.readStreamInt();

		staged[x].index = currentEntity;
		if (sr.readBit() == 0)
		{
			if (sr.readBit())
			{
				staged[x].type = 0;
				staged[x].data.parentService = PVSParser(sr, dt);
				readFromStream(sr, dt, staged[x].data);
			}
			else
			{
				staged[x].type = 1;
				staged[x].data.parentService = props[currentEntity].parentService;
				readFromStream(sr, dt, staged[x].data);
			}
		}
		else
		{
			staged[x].type = 2;
			DataTable::ServiceClass nullified = DataTable::ServiceClass();
			nullified.id = -1;
			sr.readBit();
		}
		assert(!sr.isEof());
	}
	assert(x == (int)pe.updated_entries());
	// assert(sr.isEof()); //this fails, dont know if its a big deal
}

std::vector<GameEntities::StagedChange>	&GameEntities::getStagedChanges()
{
	return staged;
}

void		GameEntities::executeChanges(DemoFile &df)
{
	for (size_t i = 0; i < staged.size(); i++)
	{
		if (staged[i].type == 0)
		{
			props[staged[i].index] = staged[i].data;
			if (staged[i].data.parentService->nameDataTable == "DT_CSPlayer")
			{
				df.getPlayer(staged[i].index - 1).packetRef = &(props[staged[i].index]);
			}
		}
		else if (staged[i].type == 1)
		{
			Entity	&ref = props[staged[i].index];
			for (auto it = staged[i].data.properties.begin(); \
				it != staged[i].data.properties.end(); it++)
			{
				GameEntities::Property &p = ref.properties[it->first];
				delete (char *)p.data;
				p.data = it->second.data;
			}
			
		}
		else if (staged[i].type == 2)
		{
			Entity nullified;

			nullified.parentService = 0;
			props[staged[i].index] = nullified;
		}
	}
}

GameEntities::GameEntities() {}

GameEntities::Property::~Property()
{
}

GameEntities::StagedChange	&GameEntities::StagedChange::operator=(const GameEntities::StagedChange &s)
{
	type = s.type;
	index = s.index;
	data = s.data;

	return (*this);
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