#include <demo.hpp>



int decodeInt(standardParameters, const SendTable_sendprop_t &prop)
{
	if (prop.flags() & ( 1 << 19))
	{
		assert(0 != 0);
		return readBits(4);
	}
	else
	{
		return readBits(prop.num_bits());
	}
}


