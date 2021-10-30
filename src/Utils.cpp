#include <demo.hpp>

unsigned int readVarInt(FILE *f, size_t *iter)
{
	unsigned int result = 0;
	char b = 0;
	char count = 0;
	do
	{
		if (count == 5)
		{
			return result;
		}
		fread(&b, 1, sizeof(b), f);
		result |= (b & 127) << (7 * count);
		//std::cout << "current: " << result << " b: " << (int)b << " count:" << (int)count << std::endl;
		count++;
		if (iter)
			(*iter)++;
	} while (b & 128);
	return result;	
}