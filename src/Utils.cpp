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

std::string	readVarString(FILE *f, size_t *iter)
{
	unsigned int len = readVarInt(f, iter);

	if (len == 0)
		return "";
	std::string rv;
	char tmp;

	for (size_t i = 0; i < len; i++)
	{
		if (iter)
			(*iter) += fread(&tmp, 1, 1, f);
		else
			fread(&tmp, 1, 1, f);
		if (tmp == 0)
			break;
		rv += tmp;
	}
	
	return rv;
}


bool	readVarBool(FILE *f, size_t *iter)
{
	return !!readVarInt(f, iter);
}

void Vector::Init(void)
{
	x = y = z = 0.0f;
}

void Vector::Init( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}