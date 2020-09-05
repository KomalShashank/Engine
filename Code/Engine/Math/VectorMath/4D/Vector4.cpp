#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Vector4::Vector4()
{

}



Vector4::Vector4(float initialX, float initialY, float initialZ, float initialT) : X(initialX), Y(initialY), Z(initialZ), T(initialT)
{

}



Vector4::Vector4(const Vector4& copyableVector4)
{
	X = copyableVector4.X;
	Y = copyableVector4.Y;
	Z = copyableVector4.Z;
	T = copyableVector4.T;
}



Vector4 Vector4::operator+(const Vector4& addableVector4) const
{
	Vector4 resultantVector4;
	resultantVector4.X = X + addableVector4.X;
	resultantVector4.Y = Y + addableVector4.Y;
	resultantVector4.Z = Z + addableVector4.Z;
	resultantVector4.T = T + addableVector4.T;

	return resultantVector4;
}



Vector4 Vector4::operator-(const Vector4& subtractableVector4) const
{
	Vector4 resultantVector4;
	resultantVector4.X = X - subtractableVector4.X;
	resultantVector4.Y = Y - subtractableVector4.Y;
	resultantVector4.Z = Z - subtractableVector4.Z;
	resultantVector4.T = T - subtractableVector4.T;

	return resultantVector4;
}



Vector4 Vector4::operator*(float multipliableValue) const
{
	Vector4 resultantVector4;
	resultantVector4.X = X * multipliableValue;
	resultantVector4.Y = Y * multipliableValue;
	resultantVector4.Z = Z * multipliableValue;
	resultantVector4.T = T * multipliableValue;

	return resultantVector4;
}



Vector4 Vector4::operator/(float dividableValue) const
{
	Vector4 resultantVector4;
	resultantVector4.X = X / dividableValue;
	resultantVector4.Y = Y / dividableValue;
	resultantVector4.Z = Z / dividableValue;
	resultantVector4.T = T / dividableValue;

	return resultantVector4;
}



void Vector4::operator+=(const Vector4& addableVector4)
{
	X += addableVector4.X;
	Y += addableVector4.Y;
	Z += addableVector4.Z;
	T += addableVector4.T;
}



void Vector4::operator-=(const Vector4& subtractableVector4)
{
	X -= subtractableVector4.X;
	Y -= subtractableVector4.Y;
	Z -= subtractableVector4.Z;
	T -= subtractableVector4.T;
}



void Vector4::operator*=(float multipliableValue)
{
	X *= multipliableValue;
	Y *= multipliableValue;
	Z *= multipliableValue;
	T *= multipliableValue;
}



void Vector4::operator/=(float dividableValue)
{
	X /= dividableValue;
	Y /= dividableValue;
	Z /= dividableValue;
	T /= dividableValue;
}



bool Vector4::operator==(const Vector4& comparableVector4) const
{
	return ((X == comparableVector4.X) && (Y == comparableVector4.Y) && (Z == comparableVector4.Z) && (T == comparableVector4.T));
}



bool Vector4::operator!=(const Vector4& comparableVector4) const
{
	return ((X != comparableVector4.X) || (Y != comparableVector4.Y) || (Z != comparableVector4.Z) || (T != comparableVector4.T));
}



float Vector4::GetVector4Magnitude() const
{
	return SquareRoot((X * X) + (Y * Y) + (Z * Z) + (T * T));
}



Vector4 Vector4::GetNormalizedVector4() const
{
	float vectorMagnitude = GetVector4Magnitude();

	Vector4 normalizedVector4(0.0f, 0.0f, 0.0f, 0.0f);
	if (vectorMagnitude != 0.0f)
	{
		normalizedVector4.X = X / vectorMagnitude;
		normalizedVector4.Y = Y / vectorMagnitude;
		normalizedVector4.Z = Z / vectorMagnitude;
		normalizedVector4.T = T / vectorMagnitude;
	}

	return normalizedVector4;
}



float Vector4::DotProduct(const Vector4& firstDottableVector4, const Vector4& secondDottableVector4)
{
	float resultantScalar = (firstDottableVector4.X * secondDottableVector4.X) + (firstDottableVector4.Y * secondDottableVector4.Y) + (firstDottableVector4.Z * secondDottableVector4.Z) + (firstDottableVector4.T * secondDottableVector4.T);

	return resultantScalar;
}



Vector4 Vector4::GetNegatedVector4() const
{
	Vector4 negatedVector4;
	negatedVector4.X = X * -1.0f;
	negatedVector4.Y = Y * -1.0f;
	negatedVector4.Z = Z * -1.0f;
	negatedVector4.T = T * -1.0f;

	return negatedVector4;
}



const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::X_AXIS(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::Y_AXIS(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::Z_AXIS(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::T_AXIS(0.0f, 0.0f, 0.0f, 1.0f);