#include "Engine/Math/VectorMath/3D/IntVector3.hpp"



IntVector3::IntVector3()
{

}



IntVector3::IntVector3(int initialX, int initialY, int initialZ) : X(initialX), Y(initialY), Z(initialZ)
{

}



IntVector3::IntVector3(const IntVector3& copyableIntVector3)
{
	X = copyableIntVector3.X;
	Y = copyableIntVector3.Y;
	Z = copyableIntVector3.Z;
}



IntVector3 IntVector3::operator+(const IntVector3& addableIntVector3) const
{
	IntVector3 resultantIntVector3;
	resultantIntVector3.X = X + addableIntVector3.X;
	resultantIntVector3.Y = Y + addableIntVector3.Y;
	resultantIntVector3.Z = Z + addableIntVector3.Z;

	return resultantIntVector3;
}



IntVector3 IntVector3::operator-(const IntVector3& subtractableIntVector3) const
{
	IntVector3 resultantIntVector3;
	resultantIntVector3.X = X - subtractableIntVector3.X;
	resultantIntVector3.Y = Y - subtractableIntVector3.Y;
	resultantIntVector3.Z = Z - subtractableIntVector3.Z;

	return resultantIntVector3;
}



IntVector3 IntVector3::operator*(int multipliableIntValue) const
{
	IntVector3 resultantIntVector3;
	resultantIntVector3.X = X * multipliableIntValue;
	resultantIntVector3.Y = Y * multipliableIntValue;
	resultantIntVector3.Z = Z * multipliableIntValue;

	return resultantIntVector3;
}



IntVector3 IntVector3::operator/(int dividableIntValue) const
{
	IntVector3 resultantIntVector3;
	resultantIntVector3.X = X / dividableIntValue;
	resultantIntVector3.Y = Y / dividableIntValue;
	resultantIntVector3.Z = Z / dividableIntValue;

	return resultantIntVector3;
}



void IntVector3::operator+=(const IntVector3& addableIntVector3)
{
	X += addableIntVector3.X;
	Y += addableIntVector3.Y;
	Z += addableIntVector3.Z;
}



void IntVector3::operator-=(const IntVector3& subtractableIntVector3)
{
	X -= subtractableIntVector3.X;
	Y -= subtractableIntVector3.Y;
	Z -= subtractableIntVector3.Z;
}



void IntVector3::operator*=(int multipliableIntValue)
{
	X *= multipliableIntValue;
	Y *= multipliableIntValue;
	Z *= multipliableIntValue;
}



void IntVector3::operator/=(int dividableIntValue)
{
	X /= dividableIntValue;
	Y /= dividableIntValue;
	Z /= dividableIntValue;
}



bool IntVector3::operator==(const IntVector3& comparableIntVector3) const
{
	return ((X == comparableIntVector3.X) && (Y == comparableIntVector3.Y) && (Z == comparableIntVector3.Z));
}



bool IntVector3::operator!=(const IntVector3& comparableIntVector3) const
{
	return ((X != comparableIntVector3.X) || (Y != comparableIntVector3.Y) || (Z != comparableIntVector3.Z));
}



IntVector3 IntVector3::GetNegatedIntVector3() const
{
	IntVector3 negatedIntVector3;
	negatedIntVector3.X = X * -1;
	negatedIntVector3.Y = Y * -1;
	negatedIntVector3.Z = Z * -1;

	return negatedIntVector3;
}



const IntVector3 IntVector3::ZERO(0, 0, 0);
const IntVector3 IntVector3::ONE(1, 1, 1);
const IntVector3 IntVector3::X_AXIS(1, 0, 0);
const IntVector3 IntVector3::Y_AXIS(0, 1, 0);
const IntVector3 IntVector3::Z_AXIS(0, 0, 1);