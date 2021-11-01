#include <demo.hpp>

DataTable::DataTable(FILE *f)
{
	int size = 0;

	fread(&size, sizeof(int), 1, f);
	MessageVector messages = getProtoMesssages(f, size);
}