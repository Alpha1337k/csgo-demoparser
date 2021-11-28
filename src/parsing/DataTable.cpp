#include <demo.hpp>

DataTable::DataTable(FILE *f)
{
	int size = 0;

	fread(&size, sizeof(int), 1, f);
	msg = getProtoMesssages(f, -1);

	short serverClassesCount = 0;
	fread(&serverClassesCount, sizeof(serverClassesCount), 1, f);

	short serverClassCountIter = serverClassesCount;
	serviceClassBits = 0;

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

bool	DataTable::ServiceClass::isPropExcluded(DataTable &dt, const SendTable_sendprop_t &p)
{
	for (size_t i = 0; i < dt.excludedProps.size(); i++)
	{
		if (p.var_name() == dt.excludedProps[i].var_name() && \
			p.dt_name() ==  dt.excludedProps[i].dt_name())
			return true;
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

void	DataTable::ServiceClass::iterateProps(DataTable &dt, const SendTable &send, std::vector<PropW> &store)
{
	for (size_t i = 0; i < send.props_size(); i++)
	{
		const SendTable_sendprop_t &prop = send.props(i);
		if ( ( prop.flags() & (1 << 8) ) || 
			 ( prop.flags() & (1 << 6) ) || 
			 isPropExcluded( dt, prop ) )
		{
			continue;
		}
		if (prop.type() == 6)
		{
			SendTable *st = dt.findSendTable(prop.dt_name());
			assert(st != 0);
			if (prop.flags() & (1 << 11))
			{
				iterateProps(dt, *st, store);
			}
			else
			{
				gatherProps(dt, *st);
			}
		}
		else
		{
			if (prop.type() == 5)
			{
				store.push_back(PropW(send.props(i - 1)));
			}
			else
			{
				store.push_back(PropW(prop));
			}
		}
	}
	
}


void	DataTable::ServiceClass::gatherProps(DataTable &dt, SendTable &send)
{
	std::vector<PropW> tmp;

	iterateProps(dt, send, tmp);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		props.push_back(tmp[i]);
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

	std::vector<unsigned int> priorities;

	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
		for (size_t i = 0; i < props.size() - 1; i++)
		{
			const int &p1 = props[i].prop.priority();
			const int &p2 = props[i + 1].prop.priority();
			if (p1 > p2)
			{
				PropW tmp = props[i + 1];
				props[i + 1] = props[i];
				props[i] = props[i + 1];
				isSorted = false;
			}
		}
	}
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
