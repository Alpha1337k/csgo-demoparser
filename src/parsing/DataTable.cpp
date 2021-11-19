#include <demo.hpp>

DataTable::DataTable(FILE *f)
{
	int size = 0;

	fread(&size, sizeof(int), 1, f);
	msg = getProtoMesssages(f, -1);

	short serverClassesCount = 0;
	fread(&serverClassesCount, sizeof(serverClassesCount), 1, f);

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

	return *this;
}

DataTable::DataTable() {}
