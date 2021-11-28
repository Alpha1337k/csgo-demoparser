#include <demo.hpp>

int decodeint(standardParameters, const SendTable_sendprop_t &prop)
{
	if (prop.flags() & ( 1 << 19))
	{
		exit(15);
		assert(0 != 0);
		return readBits(4);
	}
	else
	{
		return readBits(prop.num_bits());
	}
}

Vector decodeVector(standardParameters, const SendTable_sendprop_t &prop) 
{
	Vector rv;

	rv.x = decodefloat(standardIParameters, prop);
	rv.y = decodefloat(standardIParameters, prop);

	if ((prop.flags() & (1 << 5)) == 0)
		rv.z = decodefloat(standardIParameters, prop);
	else
	{
		assert(0 != 0);
		rv.z = 0;
	}
	return rv;
}
Vector2 decodeVector2(standardParameters, const SendTable_sendprop_t &prop) 
{
	Vector2 rv;

	rv.x = decodefloat(standardIParameters, prop);
	rv.y = decodefloat(standardIParameters, prop);

	return rv;
}
std::string decodestring(standardParameters, const SendTable_sendprop_t &prop) 
{

}

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

float readfBits(standardParameters)
{
	std::cerr << "FBITS" << std::endl;
	int iVal, fVal;
	float rv = 0;

	iVal = readBits(1);
	fVal = readBits(1);
	bool isNeg;

	if (iVal || fVal)
	{
		isNeg = readBits(1);

		if (iVal)
			iVal = readBits(14) + 1;
		if (fVal)
			fVal = readBits(5);
		rv = iVal + ((float)fVal * COORD_RESOLUTION);
	}

	return rv * (isNeg == 1 ? -1 : 1);
}

float readfBitsCoord(standardParameters, bool isInt, bool isLowPrc)
{
	int iVal, fVal;
	float rv = 0;
	bool isNeg, inbounds;

	std::cerr << "bitscoord" << std::endl;


	inbounds = readBits(1);

	iVal = readBits(1);

	if (iVal == 1)
	{
		isNeg = readBits(1);

		if (inbounds)
			rv = (float)readBits(11) + 1;
		else
			rv = (float)readBits(14) + 1;
	}
	if (isInt == false)
	{
		if (isLowPrc)
			fVal = readBits(3);
		else
			fVal = readBits(5);
		rv = rv + ((float)fVal * (isLowPrc ? COORD_RESOLUTION_LOWPRECISION : COORD_RESOLUTION));
	}
	return rv * (isNeg == 1 ? -1 : 1);
}

float	readFloat(standardParameters, const SendTable_sendprop_t &prop)
{
	int fl = readBits(32);

	std::cerr << "readfloat fl:" << fl << std::endl;
	return fl;
}


float decodefloat(standardParameters, const SendTable_sendprop_t &prop) 
{
	float rv;
	// kanker
	std::cout << "Flags:: " << prop.flags() << std::endl;
	if (prop.flags() & SPROP_COORD)
		rv = readfBits(standardIParameters);
	else if (prop.flags() & SPROP_COORD_MP)
		rv = readfBitsCoord(standardIParameters, false, false);
	else if (prop.flags() & SPROP_COORD_MP_LOWPRECISION)
		rv = readfBitsCoord(standardIParameters, false, true);
	else if (prop.flags() & SPROP_COORD_MP_INTEGRAL)
		rv = readfBitsCoord(standardIParameters, true, false);
	else if (prop.flags() & SPROP_NOSCALE)
		rv = readFloat(standardIParameters, prop);
	else if (prop.flags() & SPROP_NORMAL)
		assert(0 != 0);
	else if (prop.flags() & SPROP_CELL_COORD)
		assert(0 != 0);
	else if (prop.flags() & SPROP_CELL_COORD_LOWPRECISION)
		assert(0 != 0);
	else if (prop.flags() & SPROP_CELL_COORD_INTEGRAL)
		assert(0 != 0);
	else
		assert(0 != 0);
	return rv;
}