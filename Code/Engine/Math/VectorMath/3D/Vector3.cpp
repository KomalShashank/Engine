#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Vector3::Vector3()
{

}



Vector3::Vector3(float initialX, float initialY, float initialZ) : X(initialX), Y(initialY), Z(initialZ)
{

}



Vector3::Vector3(const Vector3& copyableVector3)
{
	X = copyableVector3.X;
	Y = copyableVector3.Y;
	Z = copyableVector3.Z;
}



Vector3 Vector3::operator+(const Vector3& addableVector3) const
{
	Vector3 resultantVector3;
	resultantVector3.X = X + addableVector3.X;
	resultantVector3.Y = Y + addableVector3.Y;
	resultantVector3.Z = Z + addableVector3.Z;

	return resultantVector3;
}



Vector3 Vector3::operator-(const Vector3& subtractableVector3) const
{
	Vector3 resultantVector3;
	resultantVector3.X = X - subtractableVector3.X;
	resultantVector3.Y = Y - subtractableVector3.Y;
	resultantVector3.Z = Z - subtractableVector3.Z;

	return resultantVector3;
}



Vector3 Vector3::operator*(float multipliableValue) const
{
	Vector3 resultantVector3;
	resultantVector3.X = X * multipliableValue;
	resultantVector3.Y = Y * multipliableValue;
	resultantVector3.Z = Z * multipliableValue;

	return resultantVector3;
}



Vector3 Vector3::operator/(float dividableValue) const
{
	Vector3 resultantVector3;
	resultantVector3.X = X / dividableValue;
	resultantVector3.Y = Y / dividableValue;
	resultantVector3.Z = Z / dividableValue;

	return resultantVector3;
}



void Vector3::operator+=(const Vector3& addableVector3)
{
	X += addableVector3.X;
	Y += addableVector3.Y;
	Z += addableVector3.Z;
}



void Vector3::operator-=(const Vector3& subtractableVector3)
{
	X -= subtractableVector3.X;
	Y -= subtractableVector3.Y;
	Z -= subtractableVector3.Z;
}



void Vector3::operator*=(float multipliableValue)
{
	X *= multipliableValue;
	Y *= multipliableValue;
	Z *= multipliableValue;
}



void Vector3::operator/=(float dividableValue)
{
	X /= dividableValue;
	Y /= dividableValue;
	Z /= dividableValue;
}



bool Vector3::operator==(const Vector3& comparableVector3) const
{
	return ((X == comparableVector3.X) && (Y == comparableVector3.Y) && (Z == comparableVector3.Z));
}



bool Vector3::operator!=(const Vector3& comparableVector3) const
{
	return ((X != comparableVector3.X) || (Y != comparableVector3.Y) || (Z != comparableVector3.Z));
}



float Vector3::GetVector3Magnitude() const
{
	return SquareRoot((X * X) + (Y * Y) + (Z * Z));
}



Vector3 Vector3::GetNormalizedVector3() const
{
	float vectorMagnitude = GetVector3Magnitude();

	Vector3 normalizedVector3(0.0f, 0.0f, 0.0f);
	if (vectorMagnitude != 0.0f)
	{
		normalizedVector3.X = X / vectorMagnitude;
		normalizedVector3.Y = Y / vectorMagnitude;
		normalizedVector3.Z = Z / vectorMagnitude;
	}

	return normalizedVector3;
}



float Vector3::DotProduct(const Vector3& firstDottableVector3, const Vector3& secondDottableVector3)
{
	float resultantScalar = (firstDottableVector3.X * secondDottableVector3.X) + (firstDottableVector3.Y * secondDottableVector3.Y) + (firstDottableVector3.Z * secondDottableVector3.Z);

	return resultantScalar;
}



Vector3 Vector3::CrossProduct(const Vector3& firstCrossableVector3, const Vector3& secondCrossableVector3)
{
	Vector3 resultantVector3;
	resultantVector3.X = (firstCrossableVector3.Y * secondCrossableVector3.Z) - (firstCrossableVector3.Z * secondCrossableVector3.Y);
	resultantVector3.Y = (firstCrossableVector3.Z * secondCrossableVector3.X) - (firstCrossableVector3.X * secondCrossableVector3.Z);
	resultantVector3.Z = (firstCrossableVector3.X * secondCrossableVector3.Y) - (firstCrossableVector3.Y * secondCrossableVector3.X);

	return resultantVector3;
}



Vector3 Vector3::GetNegatedVector3() const
{
	Vector3 negatedVector3;
	negatedVector3.X = X * -1.0f;
	negatedVector3.Y = Y * -1.0f;
	negatedVector3.Z = Z * -1.0f;

	return negatedVector3;
}



const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::X_AXIS(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Y_AXIS(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Z_AXIS(0.0f, 0.0f, 1.0f);