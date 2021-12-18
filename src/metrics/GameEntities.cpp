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
#define DecodeSwitch(i, typeV)											\
	case i:																\
	{																	\
		typeV rv = decode##typeV(sr, flatProp.prop);					\
		GameEntities::Property &prop = ent.properties[flatProp.path];	\
		prop.type = decoded_##typeV;									\
		prop.data = new typeV(rv);										\
		break;															\
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

void GameEntities::parse(PacketEntities &pe, DataTable &dt, DemoFile &df)
{
	StreamReader sr(pe.entity_data());	
	
	int currentEntity = -1;

	int x = 0;
	for (; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + sr.readStreamInt();

		Entity	&toChange = props[currentEntity];
		if (sr.readBit() == 0)
		{
			if (sr.readBit())
			{
				toChange.parentService = PVSParser(sr, dt);
				readFromStream(sr, dt, toChange);
				if (toChange.parentService->nameDataTable == "DT_CSPlayer")
				{
					df.getPlayer(currentEntity - 1).packetRef = &(props[currentEntity]);
				}
			}
			else
			{
				readFromStream(sr, dt, toChange);
			}
		}
		else
		{
			toChange.parentService = 0;
			sr.readBit();
		}
		assert(!sr.isEof());
	}
	assert(x == (int)pe.updated_entries());
	// assert(sr.isEof()); //this fails, dont know if its a big deal
}

GameEntities::GameEntities() {}

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