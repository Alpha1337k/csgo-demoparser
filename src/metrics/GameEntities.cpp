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
		/* printIfAllowed("--entitymsg", std::cout << flatProp.path << " : " << rv << std::endl); */ \
		GameEntities::Property prop; \
		prop.type = decoded_##typeV; \
		prop.data = new typeV(rv); \
		ent.properties[flatProp.path] = prop;  /* this is a copy, so its kinda inefficient (+-5us) */ \
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
	bool readNewWay = sr.readBit() == 1 ? true : false;

	std::vector<int> indicies;
	int index = -1;
	
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

	return &dt.services[serverClassId];
}

void GameEntities::parse(PacketEntities &pe, DataTable &dt)
{
	StreamReader sr(pe.entity_data());	
	
	int currentEntity = -1;

	staged.clear();
	staged.reserve(pe.updated_entries());

	int x = 0;
	for (; x < pe.updated_entries(); x++)
	{
		StagedChange *newChange = new StagedChange;

		currentEntity += 1 + sr.readStreamInt();

		newChange->index = currentEntity;
		// printIfAllowed("--entitymsg", std::c\out << "-------[Current Entity: " << currentEntity << ", bytes read: << " << i << "]" << std::endl);
		if (sr.readBit() == 0)
		{
			if (sr.readBit())
			{
				newChange->type = 0;

				newChange->data.parentService = PVSParser(sr, dt);

				// printIfAllowed("--entitymsg", std::cout << "Create" << std::endl);
				// printIfAllowed("--entitymsg", std::cout << newChange.data.parentService << std::endl);
				readFromStream(sr, dt, newChange->data);
			}
			else
			{
				newChange->type = 1;
				newChange->data.parentService = props[currentEntity].parentService;

				// printIfAllowed("--entitymsg", std::cout << "Update" << std::endl);
				// printIfAllowed("--entitymsg", std::cout << newChange.data.parentService << std::endl);
				readFromStream(sr, dt, newChange->data);
			}
		}
		else
		{
			newChange->type = 2;

			// printIfAllowed("--entitymsg", std::cout << "Delete" << std::endl);
			// printIfAllowed("--entitymsg", std::cout << props[currentEntity].parentService << std::endl);

			DataTable::ServiceClass nullified = DataTable::ServiceClass();
			nullified.id = -1;

			sr.readBit();
		}

		staged.push_back(newChange);

		assert(!sr.isEof());
	}
	assert(x == (int)pe.updated_entries());
	// assert(i == data.length()); this fails, dont know if its a big deal
}

std::vector<GameEntities::StagedChange *>	&GameEntities::getStagedChanges()
{
	return staged;
}

void		GameEntities::executeChanges()
{
	for (size_t i = 0; i < staged.size(); i++)
	{
		if (staged[i]->type == 0)
		{
			props[staged[i]->index] = staged[i]->data;
		}
		else if (staged[i]->type == 1)
		{
			Entity	&ref = props[staged[i]->index];
			for (std::map<std::string, Property>::iterator it = staged[i]->data.properties.begin(); \
				it != staged[i]->data.properties.end(); it++)
			{
				GameEntities::Property &p = ref.properties[it->first];
				delete (char *)p.data;
				p.data = it->second.data;
			}
			
		}
		else if (staged[i]->type == 2)
		{
			Entity nullified;

			props[staged[i]->index] = nullified;
		}
	}
	for (size_t i = 0; i < staged.size(); i++)
	{
		delete staged[i];
	}
	staged.clear();
}

GameEntities::GameEntities() {}

GameEntities::Property::~Property()
{
}


/*
	kanker
*/

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