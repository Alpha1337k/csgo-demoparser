#include <demo.hpp>

StreamReader::StreamReader(const std::string &d): data(d), idx(0), bitsAvailable(8), buffer(data[0])
{
}

int		StreamReader::readBits(char len)
{
	int rval = 0;

	for (int x = 0; x < len; x++)
	{
		if (bitsAvailable == 0)
		{
			buffer = data[++idx];
			bitsAvailable = 8;
		}
		rval |= ((buffer >> (8 - bitsAvailable--)) & 1) << x;
	}
	return rval;
}

int		StreamReader::readStreamInt()
{
	int ret = readBits(6);
	switch (ret & (16 | 32)) {
		case 16:
			ret = (ret & 15) | (readBits(4) << 4);
			break;
		case 32:
			ret = (ret & 15) | (readBits(8) << 4);
			break;
		case 48:
			ret = (ret & 15) | (readBits(28) << 4);
			break;
	}
	return ret;
}

bool	StreamReader::isEof()
{
	return idx >= data.length();
}
