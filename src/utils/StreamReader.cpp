#include <demo.hpp>

const unsigned int s_nMaskTable[33] = {
	0,
	( 1 << 1 ) - 1,
	( 1 << 2 ) - 1,
	( 1 << 3 ) - 1,
	( 1 << 4 ) - 1,
	( 1 << 5 ) - 1,
	( 1 << 6 ) - 1,
	( 1 << 7 ) - 1,
	( 1 << 8 ) - 1,
	( 1 << 9 ) - 1,
	( 1 << 10 ) - 1,
	( 1 << 11 ) - 1,
	( 1 << 12 ) - 1,
	( 1 << 13 ) - 1,
	( 1 << 14 ) - 1,
	( 1 << 15 ) - 1,
	( 1 << 16 ) - 1,
	( 1 << 17 ) - 1,
	( 1 << 18 ) - 1,
	( 1 << 19 ) - 1,
   	( 1 << 20 ) - 1,
	( 1 << 21 ) - 1,
	( 1 << 22 ) - 1,
	( 1 << 23 ) - 1,
	( 1 << 24 ) - 1,
	( 1 << 25 ) - 1,
	( 1 << 26 ) - 1,
   	( 1 << 27 ) - 1,
	( 1 << 28 ) - 1,
	( 1 << 29 ) - 1,
	( 1 << 30 ) - 1,
	0x7fffffff,
	0xffffffff,
};

StreamReader::StreamReader(const std::string &d): data(d), idx(0), bitsAvailable(32), buffer(*(int *)&data[idx])
{
	int diff = data.length() % 4;

	if (d.length() > 4 && diff != 0)
	{
		idx = diff - 4;
		bitsAvailable = diff * 8;
		buffer = (buffer << (32 - bitsAvailable)) >> (32 - bitsAvailable);
	}
	else if (d.length() <= 4)
		bitsAvailable = d.length() * 8;
	else
		bitsAvailable = 32;
}

void	StreamReader::loadBuffer()
{
	idx += sizeof(buffer);
	buffer = *(int *)&data[idx];
	bitsAvailable = 32;
}

int		StreamReader::readBits(int len)
{
	if (bitsAvailable >= len)
	{
		int rv = buffer & s_nMaskTable[len];
		bitsAvailable -= len;
		if (bitsAvailable)
			buffer >>= len;
		else
			loadBuffer();
		return rv;
	}
	else
	{
		unsigned int rv = buffer;
		len -= bitsAvailable;
		idx += sizeof(buffer);
		buffer = *(int *)&data[idx];
		rv |= ((buffer & s_nMaskTable[len]) << bitsAvailable);
		bitsAvailable = 32 - len;
		buffer >>= len;

		return rv;
	}
}

int		StreamReader::readBit()
{
	int rv = buffer & 1;

	bitsAvailable--;
	if (bitsAvailable == 0)
	{
		loadBuffer();
	}
	else
	{
		buffer >>= 1;
	}
	return rv;
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
	return idx >= (int)data.length();
}
