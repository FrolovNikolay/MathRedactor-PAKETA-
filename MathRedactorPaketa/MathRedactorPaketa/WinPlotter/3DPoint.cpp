#include "3DPoint.h"
#include <cmath>

C3DPoint::C3DPoint( double _x, double _y, double _z ) :
X(_x), Y(_y), Z(_z)
{
}

C3DPoint C3DPoint::operator+( const C3DPoint& other ) const
{
	return C3DPoint( X + other.X, Y + other.Y, Z + other.Z );
}

void C3DPoint::operator+=( const C3DPoint& other )
{
	X += other.X;
	Y += other.Y;
	Z += other.Z;
}

C3DPoint C3DPoint::operator-( const C3DPoint& other ) const
{
	return C3DPoint( X - other.X, Y - other.Y, Z - other.Z );
}

void C3DPoint::operator-= (const C3DPoint& other )
{
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
}

C3DPoint C3DPoint::operator*( double value ) const
{
	return C3DPoint( X * value, Y * value, Z * value );
}

void C3DPoint::operator*=( double value )
{
	X *= value;
	Y *= value;
	Z *= value;
}

// friend-функция для класса C3DPoint
C3DPoint operator*(double value, const C3DPoint& other)
{
	return other * value;
}

C3DPoint C3DPoint::operator/( double value ) const
{
	return C3DPoint(X / value, Y / value, Z / value);
}

void C3DPoint::operator/=(double value)
{
	X /= value;
	Y /= value;
	Z /= value;
}

double C3DPoint::length() const {
	return sqrt(X * X + Y * Y + Z * Z);
}

C3DPoint C3DPoint::normalize() const {
	double len = length();
	if ( len < 1e-9 ) {
		throw C3DPoint::CNullLengthException();
	}
	return C3DPoint(X / len, Y / len, Z / len);
}

C3DPoint C3DPoint::cross(const C3DPoint& otherPoint) const
{
	C3DPoint result;
	
	// Значение X для вектора - (V1.y * V2.z) - (V1.z * V2.y)
	result.X = ((Y * otherPoint.Z) - (Z * otherPoint.Y));

	// Значение Y - (V1.z * V2.x) - (V1.x * V2.z)
	result.Y = ((Z * otherPoint.X) - (X * otherPoint.Z));

	// Значение Z: (V1.x * V2.y) - (V1.y * V2.x)
	result.Z = ((X * otherPoint.Y) - (Y * otherPoint.X));

	return result;
}

double C3DPoint::dot(const C3DPoint& otherPoint) const
{
	return X * otherPoint.X + Y * otherPoint.Y + Z * otherPoint.Z;
}