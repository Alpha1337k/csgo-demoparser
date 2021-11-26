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
		if (sc.id > serverClassesCount)
		{
			std::cerr << "Error: id is bigger than serverclasscount" << std::endl;
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

void	DataTable::ServiceClass::setProps(DataTable &dt, SendTable *send)
{
	if (!dataTable || !dataTable->props_size())
		return;
	if (send == 0)
		send = dataTable;
	const SendTable &st = *send;
	std::string path = "";

	for (size_t i = 0; i < st.props_size(); i++)
	{
		const SendTable_sendprop_t &prop = st.props(i);

		if (prop.flags() & (1 << 8) || prop.flags() & (1 << 6) || prop.type() == 6)
			continue;
		std::string propPath = prop.var_name() == "baseclass" ? "" : prop.var_name(); 
		if (propPath != "" && path != "")
			propPath = path + '.' + propPath;
		
		else
		{
			PropW p;
			p.prop = prop;
			p.path = propPath;

			props.push_back(p);
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
