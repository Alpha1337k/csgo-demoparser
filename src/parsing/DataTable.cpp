#include <demo.hpp>

DataTable::DataTable(FILE *f)
{
	//int size = 0;

	//fread(&size, sizeof(int), 1, f);
	std::cout << "Starting to read datatable!" << std::endl;
	MessageVector messages = getProtoMesssages(f, -1);

	// std::cout << "Fully parsed datatable! size was : " << 1 << std::endl;

	short serverClassesCount = 0;
	fread(&serverClassesCount, sizeof(serverClassesCount), 1, f);

	// std::cout << "Count: " << serverClassesCount << std::endl;
	for (short i = 0; i < serverClassesCount; i++)
	{
		short id = 0;
		fread(&id, sizeof(id), 1, f);
		std::string s1 = readVarString(f, NULL);
		std::string s2 = readVarString(f, NULL);


		// std::cout << id << ", " << s1 << ", " << s2 <<  std::endl;
	}
	
}