#include <demo.hpp>

int readVarInt(FILE *f, size_t *iter)
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
	std::string rv;
	char tmp;

	while (1)
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

std::string	readVarString(const std::string &str, int &iter)
{
	std::string rv;
	char tmp;

	while (1)
	{
		tmp = str[iter++];
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

int		readStringBits(const std::string &str, int count, int &i, char &bitsAvailable)
{
	if (count == 8 && bitsAvailable == 0)
		return str[i++];
	int rval = 0;
	char buffer = str[i];

	for (size_t x = 0; x < count; x++)
	{
		if (bitsAvailable == 0)
		{
			buffer = str[i++];
			bitsAvailable = 8;
		}
		rval |= ((buffer >> (8 - bitsAvailable--)) & 1) << x;
	}
	return rval;
}