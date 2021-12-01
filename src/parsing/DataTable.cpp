#include <demo.hpp>

DataTable::DataTable(FILE *f)
{
	int size = 0;

	fread(&size, sizeof(int), 1, f);
	msg = getProtoMesssages(f, -1);

	short serverClassesCount = 0;
	fread(&serverClassesCount, sizeof(serverClassesCount), 1, f);

	short serverClassCountIter = serverClassesCount;
	serviceClassBits = 1;

	while (serverClassCountIter >>= 1)
		serviceClassBits++;
	
	ServiceClass sc;
	// std::cout << "Count: " << serverClassesCount << std::endl;
	for (short i = 0; i < serverClassesCount; i++)
	{
		sc.id = 0;
		fread(&sc.id, sizeof(sc.id), 1, f);
		if (sc.id >= serverClassesCount)
		{
			std::cerr << "Error: id is bigger than serverclasscount" << std::endl;
			exit(-1);
		}
		else
		{
			sc.name = readVarString(f, NULL);
			sc.nameDataTable = readVarString(f, NULL);
			services.push_back(sc);
		}
	}	
}

DataTable &DataTable::operator=(const DataTable &d)
{
	services = d.services;
	msg = d.msg;
	serviceClassBits = d.serviceClassBits;

	std::cerr << "operator called!!!" << std::endl;
	return *this;
}

DataTable::DataTable() {}

SendTable	*DataTable::findSendTable(std::string name)
{
	for (size_t i = 0; i < msg.size(); i++)
	{
		SendTable *st = (SendTable *)msg[i].second;

		if (st->net_table_name() == name)
			return st;
	}
	return 0;
}

bool	DataTable::ServiceClass::isPropExcluded(const DataTable &dt, const SendTable &st, const SendTable_sendprop_t &p)
{
	for (size_t i = 0; i < dt.excludedProps.size(); i++)
	{
		if (p.var_name() == dt.excludedProps[i].var_name() && \
			dt.excludedProps[i].dt_name() == st.net_table_name())
		{
			return true;
		}
	}
	return false;
}

void	DataTable::ServiceClass::gatherExcludes(DataTable &dt, const SendTable &send)
{
	for (size_t i = 0; i < send.props_size(); i++)
	{
		const SendTable_sendprop_t &prop = send.props(i);

		if (prop.flags() & (1 << 6))
		{
			dt.excludedProps.push_back(prop);
		}
		if (prop.type() == 6)
		{
			SendTable *st = dt.findSendTable(prop.dt_name());
			assert(st != 0);
			gatherExcludes(dt, *st);
		}
	}
}

void	DataTable::ServiceClass::iterateProps(DataTable &dt, const SendTable &send, \
											std::vector<PropW> &store, std::string path)
{
	for (size_t i = 0; i < send.props_size(); i++)
	{
		const SendTable_sendprop_t &prop = send.props(i);
		if ( ( prop.flags() & (1 << 8) ) || 
			 ( prop.flags() & (1 << 6) ) || 
			 isPropExcluded(dt, send, prop))
		{
			continue;
		}

		std::string propPath;
		if (prop.var_name() != "baseclass")
			propPath = prop.var_name();
		if (propPath.length() && path.length())
			propPath = path + '.' + propPath;
		
		if (prop.type() == 6)
		{
			SendTable *st = dt.findSendTable(prop.dt_name());
			assert(st != 0);
			if (prop.flags() & (1 << 11))
			{
				iterateProps(dt, *st, store, propPath);
			}
			else
			{
				gatherProps(dt, *st, propPath);
			}
		}
		else
		{
			if (prop.type() == 5)
			{
				store.push_back(PropW(prop, propPath));
			}
			else
			{
				store.push_back(PropW(prop, propPath));
			}
		}
	}
	
}


void	DataTable::ServiceClass::gatherProps(DataTable &dt, SendTable &send, std::string path)
{
	std::vector<PropW> tmp;

	iterateProps(dt, send, tmp, path);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		props.push_back(tmp[i]);
	}
}

void	DataTable::ServiceClass::sortProps()
{
	std::vector<unsigned int> priorities;

	priorities.push_back(64);
	for (size_t i = 0; i < props.size(); i++)
	{
		size_t x = 0;
		for (; x < priorities.size() && priorities[x] != props[i].prop.priority(); x++);

		if (x == priorities.size())
			priorities.push_back(props[i].prop.priority());
	}
	std::sort(priorities.begin(), priorities.end());

	int start = 0;
	for (size_t prior_idx = 0; prior_idx < priorities.size(); prior_idx++)
	{
		unsigned int &prior = priorities[prior_idx];

		while (1)
		{
			int cur_prop = start;
			while (cur_prop < props.size())
			{
				PropW &p = props[cur_prop];
				if (p.prop.priority() == prior || (prior == 64 && ((1 << 18) & p.prop.flags())))
				{
					if (start != cur_prop)
					{
						PropW tmp = props[start];
						props[start] = props[cur_prop];
						props[cur_prop] = tmp;
					}
					start++;
					break;
				}
				cur_prop++;
			}
			if (cur_prop == props.size())
				break;
		}
	}
}

void	DataTable::ServiceClass::flattenProps(DataTable &dt, SendTable *send)
{
	if (!dataTable || !dataTable->props_size())
		return;
	if (send == 0)
		send = dataTable;
	SendTable &st = *send;
	std::string path = "";
	dt.excludedProps.clear();
	gatherExcludes(dt, st);
	
	gatherProps(dt, st);
	sortProps();

}

DataTable::ServiceClass &DataTable::ServiceClass::operator=(const DataTable::ServiceClass &cs)
{
	id = cs.id;
	name = cs.name;
	nameDataTable = cs.nameDataTable;
	dataTable = cs.dataTable;
	props = cs.props;

	return (*this);
}
