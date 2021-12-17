#include <demo.hpp>

const int SPROP_COORD = (1 << 1);
const int SPROP_NOSCALE = (1 << 2);
const int SPROP_NORMAL = (1 << 5);
const int SPROP_COORD_MP = (1 << 12);
const int SPROP_COORD_MP_LOWPRECISION = (1 << 13);
const int SPROP_COORD_MP_INTEGRAL = (1 << 14);
const int SPROP_CELL_COORD = (1 << 15);
const int SPROP_CELL_COORD_LOWPRECISION = (1 << 16);
const int SPROP_CELL_COORD_INTEGRAL = (1 << 17);

const int COORD_FRACTIONAL_BITS = 5;
const int COORD_DENOMINATOR = ( 1 << ( COORD_FRACTIONAL_BITS ) );
const float COORD_RESOLUTION = ( 1.0f / ( COORD_DENOMINATOR ) );

const int COORD_FRACTIONAL_BITS_MP_LOWPRECISION = 3;
const float COORD_DENOMINATOR_LOWPRECISION = ( 1 << ( COORD_FRACTIONAL_BITS_MP_LOWPRECISION ) );
const float COORD_RESOLUTION_LOWPRECISION = ( 1.0f / ( COORD_DENOMINATOR_LOWPRECISION ) );

int decodeint(StreamReader &sr, const SendTable_sendprop_t &prop)
{
	if (prop.flags() & ( 1 << 19))
	{
		assert(0 != 0);
		return sr.readBits(4);
	}
	else
	{
		return sr.readBits(prop.num_bits());
	}
}

Vector decodeVector(StreamReader &sr, const SendTable_sendprop_t &prop) 
{
	Vector rv;

	rv.x = decodefloat(sr, prop);
	rv.y = decodefloat(sr, prop);

	if ((prop.flags() & (1 << 5)) == 0)
		rv.z = decodefloat(sr, prop);
	else
	{
		rv.z = rv.x * rv.x + rv.y * rv.y;
		if (rv.z < 1)
			rv.z = sqrt(1 - rv.z);
		else
			rv.z = 0;

		if (sr.readBit())
			rv.z = -1;
	}
	return rv;
}
Vector2 decodeVector2(StreamReader &sr, const SendTable_sendprop_t &prop) 
{
	Vector2 rv;

	rv.x = decodefloat(sr, prop);
	rv.y = decodefloat(sr, prop);

	return rv;
}
std::string decodestring(StreamReader &sr, const SendTable_sendprop_t &prop)
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
	int iVal, fVal;
	float rv = 0;

	iVal = sr.readBit();
	fVal = sr.readBit();
	bool isNeg = false;

	if (iVal || fVal)
	{
		isNeg = sr.readBit();

		if (iVal)
			iVal = sr.readBits(14) + 1;
		if (fVal)
			fVal = sr.readBits(5);
		rv = iVal + ((float)fVal * COORD_RESOLUTION);
	}

	return isNeg ? -rv : rv;
}

float readfBitsCoord(StreamReader &sr, bool isInt, bool isLowPrc)
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
	if (isInt == false)
	{
		if (isLowPrc)
			fVal = sr.readBits(3);
		else
			fVal = sr.readBits(5);
		rv = rv + ((float)fVal * (isLowPrc ? COORD_RESOLUTION_LOWPRECISION : COORD_RESOLUTION));
	}
	return isNeg ? -rv : rv;
}

float	readFloat(StreamReader &sr)
{
	unsigned int fl = sr.readBits(32);

	return *((float *)&fl);
}

float readfCellCoord(StreamReader &sr, const SendTable_sendprop_t &prop, char level)
{
	if (level == 2)
		return sr.readBits(prop.num_bits());
	int iVal = sr.readBits(prop.num_bits());
	int fVal = sr.readBits(level == 1 ? 3 : 5);

	return iVal + (fVal * (1 / (1 << (level == 1 ? 3 : 5))));
}

float readfIntep(StreamReader &sr, const SendTable_sendprop_t &prop)
{
	float rv = 0;

	int fl = sr.readBits(prop.num_bits());

	rv = (float)fl / ((1 << prop.num_bits()) - 1);
	rv = prop.low_value() + (prop.high_value() - prop.low_value()) * rv;

	return rv;
}

float readfNormal(StreamReader &sr)
{
	int sign = sr.readBit();
	int	dt = sr.readBits(11);

	float val = (float)dt * (1 / ((1 << 11) - 1));

	return sign == 1 ? -val : val;
}

float decodefloat(StreamReader &sr, const SendTable_sendprop_t &prop) 
{
	if (prop.flags() & SPROP_COORD)
		return readfBits(sr);
	else if (prop.flags() & SPROP_COORD_MP)
		return readfBitsCoord(sr, false, false);
	else if (prop.flags() & SPROP_COORD_MP_LOWPRECISION)
		return readfBitsCoord(sr, false, true);
	else if (prop.flags() & SPROP_COORD_MP_INTEGRAL)
		return readfBitsCoord(sr, true, false);
	else if (prop.flags() & SPROP_NOSCALE)
		return readFloat(sr);
	else if (prop.flags() & SPROP_NORMAL)
		return readfNormal(sr);
	else if (prop.flags() & SPROP_CELL_COORD)
		return readfCellCoord(sr, prop, 0);
	else if (prop.flags() & SPROP_CELL_COORD_LOWPRECISION)
		return readfCellCoord(sr, prop, 1);
	else if (prop.flags() & SPROP_CELL_COORD_INTEGRAL)
		return readfCellCoord(sr, prop, 2);
	else
		return readfIntep(sr, prop);
}