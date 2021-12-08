#include <demo.hpp>

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
