#include "Engine/Math/VectorMath/2D/IntVector2.hpp"



IntVector2::IntVector2()
{

}



IntVector2::IntVector2(int initialX, int initialY) : X(initialX), Y(initialY)
{

}



IntVector2::IntVector2(const IntVector2& copyableIntVector2)
{
	X = copyableIntVector2.X;
	Y = copyableIntVector2.Y;
}



IntVector2 IntVector2::operator+(const IntVector2& addableIntVector2) const
{
	IntVector2 resultantIntVector2;
	resultantIntVector2.X = X + addableIntVector2.X;
	resultantIntVector2.Y = Y + addableIntVector2.Y;

	return resultantIntVector2;
}



IntVector2 IntVector2::operator-(const IntVector2& subtractableIntVector2) const
{
	IntVector2 resultantIntVector2;
	resultantIntVector2.X = X - subtractableIntVector2.X;
	resultantIntVector2.Y = Y - subtractableIntVector2.Y;

	return resultantIntVector2;
}



IntVector2 IntVector2::operator*(int multipliableIntValue) const
{
	IntVector2 resultantIntVector2;
	resultantIntVector2.X = X * multipliableIntValue;
	resultantIntVector2.Y = Y * multipliableIntValue;

	return resultantIntVector2;
}



IntVector2 IntVector2::operator/(int dividableIntValue) const
{
	IntVector2 resultantIntVector2;
	resultantIntVector2.X = X / dividableIntValue;
	resultantIntVector2.Y = Y / dividableIntValue;

	return resultantIntVector2;
}



void IntVector2::operator+=(const IntVector2& addableIntVector2)
{
	X += addableIntVector2.X;
	Y += addableIntVector2.Y;
}



void IntVector2::operator-=(const IntVector2& subtractableIntVector2)
{
	X -= subtractableIntVector2.X;
	Y -= subtractableIntVector2.Y;
}



void IntVector2::operator*=(int multipliableIntValue)
{
	X *= multipliableIntValue;
	Y *= multipliableIntValue;
}



void IntVector2::operator/=(int dividableIntValue)
{
	X /= dividableIntValue;
	Y /= dividableIntValue;
}



bool IntVector2::operator==(const IntVector2& comparableIntVector2) const
{
	return ((X == comparableIntVector2.X) && (Y == comparableIntVector2.Y));
}



bool IntVector2::operator!=(const IntVector2& comparableIntVector2) const
{
	return ((X != comparableIntVector2.X) || (Y != comparableIntVector2.Y));
}



IntVector2 IntVector2::GetNegatedIntVector2() const
{
	IntVector2 negatedIntVector2;
	negatedIntVector2.X = X * -1;
	negatedIntVector2.Y = Y * -1;

	return negatedIntVector2;
}



const IntVector2 IntVector2::ZERO(0, 0);
const IntVector2 IntVector2::ONE(1, 1);
const IntVector2 IntVector2::X_AXIS(1, 0);
const IntVector2 IntVector2::Y_AXIS(0, 1);