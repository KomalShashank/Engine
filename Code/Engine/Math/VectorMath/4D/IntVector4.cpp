#include "Engine/Math/VectorMath/4D/IntVector4.hpp"



IntVector4::IntVector4()
{

}



IntVector4::IntVector4(int initialX, int initialY, int initialZ, int initialT) : X(initialX), Y(initialY), Z(initialZ), T(initialT)
{

}



IntVector4::IntVector4(const IntVector4& copyableIntVector4)
{
	X = copyableIntVector4.X;
	Y = copyableIntVector4.Y;
	Z = copyableIntVector4.Z;
	T = copyableIntVector4.T;
}



IntVector4 IntVector4::operator+(const IntVector4& addableIntVector4) const
{
	IntVector4 resultantIntVector4;
	resultantIntVector4.X = X + addableIntVector4.X;
	resultantIntVector4.Y = Y + addableIntVector4.Y;
	resultantIntVector4.Z = Z + addableIntVector4.Z;
	resultantIntVector4.T = T + addableIntVector4.T;

	return resultantIntVector4;
}



IntVector4 IntVector4::operator-(const IntVector4& subtractableIntVector4) const
{
	IntVector4 resultantIntVector4;
	resultantIntVector4.X = X - subtractableIntVector4.X;
	resultantIntVector4.Y = Y - subtractableIntVector4.Y;
	resultantIntVector4.Z = Z - subtractableIntVector4.Z;
	resultantIntVector4.T = T - subtractableIntVector4.T;

	return resultantIntVector4;
}



IntVector4 IntVector4::operator*(int multipliableValue) const
{
	IntVector4 resultantIntVector4;
	resultantIntVector4.X = X * multipliableValue;
	resultantIntVector4.Y = Y * multipliableValue;
	resultantIntVector4.Z = Z * multipliableValue;
	resultantIntVector4.T = T * multipliableValue;

	return resultantIntVector4;
}



IntVector4 IntVector4::operator/(int dividableValue) const
{
	IntVector4 resultantIntVector4;
	resultantIntVector4.X = X / dividableValue;
	resultantIntVector4.Y = Y / dividableValue;
	resultantIntVector4.Z = Z / dividableValue;
	resultantIntVector4.T = T / dividableValue;

	return resultantIntVector4;
}



void IntVector4::operator+=(const IntVector4& addableIntVector4)
{
	X += addableIntVector4.X;
	Y += addableIntVector4.Y;
	Z += addableIntVector4.Z;
	T += addableIntVector4.T;
}



void IntVector4::operator-=(const IntVector4& subtractableIntVector4)
{
	X -= subtractableIntVector4.X;
	Y -= subtractableIntVector4.Y;
	Z -= subtractableIntVector4.Z;
	T -= subtractableIntVector4.T;
}



void IntVector4::operator*=(int multipliableValue)
{
	X *= multipliableValue;
	Y *= multipliableValue;
	Z *= multipliableValue;
	T *= multipliableValue;
}



void IntVector4::operator/=(int dividableValue)
{
	X /= dividableValue;
	Y /= dividableValue;
	Z /= dividableValue;
	T /= dividableValue;
}



bool IntVector4::operator==(const IntVector4& comparableIntVector4) const
{
	return ((X == comparableIntVector4.X) && (Y == comparableIntVector4.Y) && (Z == comparableIntVector4.Z) && (T == comparableIntVector4.T));
}



bool IntVector4::operator!=(const IntVector4& comparableIntVector4) const
{
	return ((X != comparableIntVector4.X) || (Y != comparableIntVector4.Y) || (Z != comparableIntVector4.Z) || (T != comparableIntVector4.T));
}



IntVector4 IntVector4::GetNegatedIntVector4() const
{
	IntVector4 negatedIntVector4;
	negatedIntVector4.X = X * -1;
	negatedIntVector4.Y = Y * -1;
	negatedIntVector4.Z = Z * -1;
	negatedIntVector4.T = T * -1;

	return negatedIntVector4;
}



const IntVector4 IntVector4::ZERO(0, 0, 0, 0);
const IntVector4 IntVector4::ONE(1, 1, 1, 1);
const IntVector4 IntVector4::X_AXIS(1, 0, 0, 0);
const IntVector4 IntVector4::Y_AXIS(0, 1, 0, 0);
const IntVector4 IntVector4::Z_AXIS(0, 0, 1, 0);
const IntVector4 IntVector4::T_AXIS(0, 0, 0, 1);