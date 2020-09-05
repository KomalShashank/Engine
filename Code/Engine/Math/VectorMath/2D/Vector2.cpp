#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Vector2::Vector2()
{

}



Vector2::Vector2(float initialX, float initialY) : X(initialX), Y(initialY)
{

}



Vector2::Vector2(const Vector2& copyableVector2)
{
	X = copyableVector2.X;
	Y = copyableVector2.Y;
}



Vector2 Vector2::operator+(const Vector2& addableVector2) const
{
	Vector2 resultantVector2;
	resultantVector2.X = X + addableVector2.X;
	resultantVector2.Y = Y + addableVector2.Y;

	return resultantVector2;
}



Vector2 Vector2::operator-(const Vector2& subtractableVector2) const
{
	Vector2 resultantVector2;
	resultantVector2.X = X - subtractableVector2.X;
	resultantVector2.Y = Y - subtractableVector2.Y;

	return resultantVector2;
}



Vector2 Vector2::operator*(float multipliableValue) const
{
	Vector2 resultantVector2;
	resultantVector2.X = X * multipliableValue;
	resultantVector2.Y = Y * multipliableValue;

	return resultantVector2;
}



Vector2 Vector2::operator/(float dividableValue) const
{
	Vector2 resultantVector2;
	resultantVector2.X = X / dividableValue;
	resultantVector2.Y = Y / dividableValue;

	return resultantVector2;
}



void Vector2::operator+=(const Vector2& addableVector2)
{
	X += addableVector2.X;
	Y += addableVector2.Y;
}



void Vector2::operator-=(const Vector2& subtractableVector2)
{
	X -= subtractableVector2.X;
	Y -= subtractableVector2.Y;
}



void Vector2::operator*=(float multipliableValue)
{
	X *= multipliableValue;
	Y *= multipliableValue;
}



void Vector2::operator/=(float dividableValue)
{
	X /= dividableValue;
	Y /= dividableValue;
}



bool Vector2::operator==(const Vector2& comparableVector2) const
{
	return ((X == comparableVector2.X) && (Y == comparableVector2.Y));
}



bool Vector2::operator!=(const Vector2& comparableVector2) const
{
	return ((X != comparableVector2.X) || (Y != comparableVector2.Y));
}



float Vector2::GetVector2Magnitude() const
{
	return SquareRoot((X * X) + (Y * Y));
}



Vector2 Vector2::GetNormalizedVector2() const
{
	float vectorMagnitude = GetVector2Magnitude();

	Vector2 normalizedVector2(0.0f, 0.0f);
	if (vectorMagnitude != 0.0f)
	{
		normalizedVector2.X = X / vectorMagnitude;
		normalizedVector2.Y = Y / vectorMagnitude;
	}

	return normalizedVector2;
}



float Vector2::DotProduct(const Vector2& firstDottableVector2, const Vector2& secondDottableVector2)
{
	float resultantScalar = (firstDottableVector2.X * secondDottableVector2.X) + (firstDottableVector2.Y * secondDottableVector2.Y);

	return resultantScalar;
}



Vector2 Vector2::GetNegatedVector2() const
{
	Vector2 negatedVector2;
	negatedVector2.X = X * -1.0f;
	negatedVector2.Y = Y * -1.0f;

	return negatedVector2;
}



const Vector2 Vector2::ZERO(0.0f, 0.0f);
const Vector2 Vector2::ONE(1.0f, 1.0f);
const Vector2 Vector2::X_AXIS(1.0f, 0.0f);
const Vector2 Vector2::Y_AXIS(0.0f, 1.0f);