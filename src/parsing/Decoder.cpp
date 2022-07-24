#include <demo.hpp>

const int COORD_FRACTIONAL_BITS = 5;
const int COORD_DENOMINATOR = ( 1 << ( COORD_FRACTIONAL_BITS ) );
const float COORD_RESOLUTION = ( 1.0f / ( COORD_DENOMINATOR ) );

const int COORD_FRACTIONAL_BITS_MP_LOWPRECISION = 3;
const float COORD_DENOMINATOR_LOWPRECISION = ( 1 << ( COORD_FRACTIONAL_BITS_MP_LOWPRECISION ) );
const float COORD_RESOLUTION_LOWPRECISION = ( 1.0f / ( COORD_DENOMINATOR_LOWPRECISION ) );

int decodeint(StreamReader &sr, const GameEntities::Property &prop)
{
	if (prop.flags & ( 1 << 19 ))
	{
		assert(0);
		return sr.readBits(4);
	}
	else
	{
		return sr.readBits(prop.num_bits);
	}
}

Vector decodeVector(StreamReader &sr, const GameEntities::Property &prop) 
{
	Vector rv;

	rv.x = decodefloat(sr, prop);
	rv.y = decodefloat(sr, prop);

	if ((prop.flags & (1 << 5)) == 0)
		rv.z = decodefloat(sr, prop);
	else
	{
		rv.z = rv.x * rv.x + rv.y * rv.y;
		if (rv.z < 1)
			rv.z = sqrt(1 - rv.z);
		else
			rv.z = 0;

		if (sr.readBit())
			rv.z *= -1;
	}
	return rv;
}
Vector2 decodeVector2(StreamReader &sr, const GameEntities::Property &prop) 
{
	Vector2 rv;

	rv.x = decodefloat(sr, prop);
	rv.y = decodefloat(sr, prop);

	return rv;
}
std::string decodestring(StreamReader &sr, const GameEntities::Property &prop)
{

	(void)prop;
	unsigned int len = sr.readBits(9);

	unsigned int maxBuffer = (1 << 9);
	if (len >= maxBuffer)
		len = maxBuffer - 1;

	std::string rv;

	rv.reserve(len);
	for (size_t x = 0; x < len; x++)
	{
		rv += (char)sr.readBits(8);
	}
	return rv;
}

float readfBits(StreamReader &sr)
{
	float rv;

    int flags = sr.readBits(2);
	
    if (flags == 0)
        return 0;
	bool isNeg = sr.readBit();

    if (flags == 1)
        rv = sr.readBits(14) + 1;
    else if (flags == 2)
        rv = sr.readBits(5) * 0.0315;
    else
        rv = sr.readBits(14) + 1 + sr.readBits(5) * 0.0315;
	return isNeg ? -rv : rv;
}

float readfBitsCoord(StreamReader &sr, int flags)
{
	int iVal, fVal;
	float rv = 0;
	bool isNeg = false, inbounds;

	inbounds = sr.readBit();

	iVal = sr.readBit();

	if (iVal == 1)
	{
		isNeg = sr.readBit();

		if (inbounds)
			rv = (float)sr.readBits(11) + 1;
		else
			rv = (float)sr.readBits(14) + 1;
	}
	if (flags != 2)
	{
		if (flags == 1)
			fVal = sr.readBits(3);
		else
			fVal = sr.readBits(5);
		rv = rv + ((float)fVal * (flags == 1 ? COORD_RESOLUTION_LOWPRECISION : COORD_RESOLUTION));
	}
	return isNeg ? -rv : rv;
}

float	readFloat(StreamReader &sr)
{
	unsigned int fl = sr.readBits(32);

	return *((float *)&fl);
}

float readfCellCoord(StreamReader &sr, const GameEntities::Property &prop, char level)
{
	if (level == 2)
		return sr.readBits(prop.num_bits);
	int iVal = sr.readBits(prop.num_bits);
	int fVal = sr.readBits(level == 1 ? 3 : 5);

	return iVal + (fVal * (1 / (1 << (level == 1 ? 3 : 5))));
}

float readfIntep(StreamReader &sr, const GameEntities::Property &prop)
{
	float rv;

	int fl = sr.readBits(prop.num_bits);

	rv = (float)fl / ((1 << prop.num_bits) - 1);
	rv = prop.low_value + (prop.high_value - prop.low_value) * rv;

	return rv;
}

float readfNormal(StreamReader &sr)
{
	int sign = sr.readBit();
	int	dt = sr.readBits(11);

	float val = (float)dt * (1 / (float)((1 << 11) - 1));

	return sign == 1 ? -val : val;
}

float decodefloat(StreamReader &sr, const GameEntities::Property &prop) 
{
	switch (prop.flags & 258086)
	{
	case 2:
		return readfBits(sr);
	case 4:
		return readFloat(sr);
	case 32:
		return readfNormal(sr);
	case 4096:
		return readfBitsCoord(sr, 0);
	case 8192:
		return readfBitsCoord(sr, 1);
	case 16384:
		return readfBitsCoord(sr, 2);
	case 32768:
		return readfCellCoord(sr, prop, 0);
	case 65536:
		return readfCellCoord(sr, prop, 1);
	case 131072:
		return readfCellCoord(sr, prop, 2);
	default:
		return readfIntep(sr, prop);
	}
}
