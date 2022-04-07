#ifndef STREAMREADER_HPP
#define STREAMREADER_HPP

#include <string>

#ifdef USE_INTEGER
#define size_type int
#else
#define size_type long
#endif


const unsigned size_type s_nMaskTable_l[65] = {
    0,
    ( (size_type)1 << 1 ) - 1,
    ( (size_type)1 << 2 ) - 1,
    ( (size_type)1 << 3 ) - 1,
    ( (size_type)1 << 4 ) - 1,
    ( (size_type)1 << 5 ) - 1,
    ( (size_type)1 << 6 ) - 1,
    ( (size_type)1 << 7 ) - 1,
    ( (size_type)1 << 8 ) - 1,
    ( (size_type)1 << 9 ) - 1,
    ( (size_type)1 << 10 ) - 1,
    ( (size_type)1 << 11 ) - 1,
    ( (size_type)1 << 12 ) - 1,
    ( (size_type)1 << 13 ) - 1,
    ( (size_type)1 << 14 ) - 1,
    ( (size_type)1 << 15 ) - 1,
    ( (size_type)1 << 16 ) - 1,
    ( (size_type)1 << 17 ) - 1,
    ( (size_type)1 << 18 ) - 1,
    ( (size_type)1 << 19 ) - 1,
    ( (size_type)1 << 20 ) - 1,
    ( (size_type)1 << 21 ) - 1,
    ( (size_type)1 << 22 ) - 1,
    ( (size_type)1 << 23 ) - 1,
    ( (size_type)1 << 24 ) - 1,
    ( (size_type)1 << 25 ) - 1,
    ( (size_type)1 << 26 ) - 1,
    ( (size_type)1 << 27 ) - 1,
    ( (size_type)1 << 28 ) - 1,
    ( (size_type)1 << 29 ) - 1,
    ( (size_type)1 << 30 ) - 1,
#ifdef USE_INTEGER
	0x7FFFFFFF,
	0xFFFFFFFF
#else
    ( (size_type)1 << 31 ) - 1,
    ( (size_type)1 << 32 ) - 1,
    ( (size_type)1 << 33 ) - 1,
    ( (size_type)1 << 34 ) - 1,
    ( (size_type)1 << 35 ) - 1,
    ( (size_type)1 << 36 ) - 1,
    ( (size_type)1 << 37 ) - 1,
    ( (size_type)1 << 38 ) - 1,
    ( (size_type)1 << 39 ) - 1,
    ( (size_type)1 << 40 ) - 1,
    ( (size_type)1 << 41 ) - 1,
    ( (size_type)1 << 42 ) - 1,
    ( (size_type)1 << 43 ) - 1,
    ( (size_type)1 << 44 ) - 1,
    ( (size_type)1 << 45 ) - 1,
    ( (size_type)1 << 46 ) - 1,
    ( (size_type)1 << 47 ) - 1,
    ( (size_type)1 << 48 ) - 1,
    ( (size_type)1 << 49 ) - 1,
    ( (size_type)1 << 50 ) - 1,
    ( (size_type)1 << 51 ) - 1,
    ( (size_type)1 << 52 ) - 1,
    ( (size_type)1 << 53 ) - 1,
    ( (size_type)1 << 54 ) - 1,
    ( (size_type)1 << 55 ) - 1,
    ( (size_type)1 << 56 ) - 1,
    ( (size_type)1 << 57 ) - 1,
    ( (size_type)1 << 58 ) - 1,
    ( (size_type)1 << 59 ) - 1,
    ( (size_type)1 << 60 ) - 1,
    ( (size_type)1 << 61 ) - 1,
    ( (size_type)1 << 62 ) - 1,
    0x7FFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFFF
#endif
};

class StreamReader
{
private:
    const std::string &data;
    int         idx;
    char        bitsAvailable;
    unsigned size_type buffer;
    inline void     loadBuffer()
    {
        idx += sizeof(buffer);
        buffer = *(size_type *)&data[idx];
        bitsAvailable = sizeof(size_type) * 8;
    }
public:
    ~StreamReader() {}

    StreamReader(const std::string &d): 
        data(d), idx(0), bitsAvailable(64), buffer(*(size_type *)&data[idx])
    {
        int diff = data.length() % sizeof(size_type);

        if (d.length() > sizeof(size_type) && diff != 0)
        {
            idx = diff - sizeof(size_type);
            bitsAvailable = diff * 8;
            buffer = (buffer << (sizeof(size_type) * 8 - bitsAvailable)) >> (sizeof(size_type) * 8 - bitsAvailable);
        }
        else if (d.length() <= 8)
            bitsAvailable = d.length() * 8;
        else
            bitsAvailable = sizeof(size_type) * 8;
    }

    inline int          readBits(int len)
    {
        if (bitsAvailable >= len)
        {
            int rv = buffer & s_nMaskTable_l[len];
            bitsAvailable -= len;
            if (bitsAvailable) [[likely]]
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
            buffer = *(size_type *)&data[idx];
            rv |= ((buffer & s_nMaskTable_l[len]) << bitsAvailable);
            bitsAvailable = (sizeof(size_type) * 8) - len;
            buffer >>= len;

            return rv;
        }
    }

    inline int      readBit()
    {
        int rv = buffer & 1;

        bitsAvailable--;
        if (bitsAvailable == 0) [[unlikely]]
        {
            loadBuffer();
        }
        else
        {
            buffer >>= 1;
        }
        return rv;
    }

    inline bool     isEof()
    {
        return idx >= (int)data.length();
    }

		inline int			readStreamInt()
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
};

#endif