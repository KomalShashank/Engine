#pragma once

#include <stdint.h>

#define PI_VALUE 3.1415926535897932384626433832795f



class Vector2D
{
public:
	Vector2D();
	Vector2D(float initialX, float initialY);
	Vector2D(const Vector2D& copyableVector2D);

	Vector2D operator+(const Vector2D& addableVector2D) const;
	Vector2D operator-(const Vector2D& subtractableVector2D) const;
	Vector2D operator*(float multipliableValue) const;
	Vector2D operator/(float dividableValue) const;

	void operator+=(const Vector2D& addableVector2D);
	void operator-=(const Vector2D& subtractableVector2D);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Vector2D& comparableVector2D) const;
	bool operator!=(const Vector2D& comparableVector2D) const;

	float GetVector2DMagnitude() const;
	float GetSquaredVector2DMagnitude() const;
	Vector2D GetNormalizedVector2D() const;

	static float DotProduct(const Vector2D& firstDottableVector2D, const Vector2D& secondDottableVector2D);
	static float CrossProduct(const Vector2D& firstCrossableVector2D, const Vector2D& secondCrossableVector2D);
	static Vector2D CrossProduct(const Vector2D& crossableVector2D, float crossableScalar);
	static Vector2D CrossProduct(float crossableScalar, const Vector2D& crossableVector2D);

	Vector2D GetNegatedVector2D() const;

public:
	static const Vector2D ZERO;
	static const Vector2D ONE;
	static const Vector2D X_AXIS;
	static const Vector2D Y_AXIS;

public:
	float X;
	float Y;
};



class Matrix2x2
{
public:
	Matrix2x2();
	Matrix2x2(const float* matrix2x2);
	Matrix2x2(const Matrix2x2& copyableMatrix2x2);

	static Matrix2x2 IdentityMatrix2x2();
	static Matrix2x2 ZeroMatrix2x2();

	float GetDeterminant() const;
	float GetTrace() const;

	bool HasInverse(Matrix2x2& outputInverseMatrix2x2) const;
	Matrix2x2 GetTranspose() const;

	Matrix2x2 operator+(const Matrix2x2& addableMatrix2x2) const;
	Matrix2x2 operator-(const Matrix2x2& subtractableMatrix2x2) const;
	Matrix2x2 operator*(float multipliableValue) const;
	Matrix2x2 operator/(float dividableValue) const;

	void operator+=(const Matrix2x2& addableMatrix2x2);
	void operator-=(const Matrix2x2& subtractableMatrix2x2);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Matrix2x2& comparableMatrix2x2) const;
	bool operator!=(const Matrix2x2& comparableMatrix2x2) const;

	bool IsSymmetric() const;
	bool IsSkewSymmetric() const;

	bool IsOrthogonal() const;

public:
	float m_Matrix2x2[4];
};



class AABB2D
{
public:
	AABB2D();
	AABB2D(const Vector2D& initialMinimums, const Vector2D& initialMaximums);
	AABB2D(const AABB2D& copyableAABB);

	Vector2D GetAABBCenter() const;
	Vector2D GetHalfDimensions() const;

	float GetAABBPerimeter() const;
	bool ContainsAABB(const AABB2D& containableAABB) const;

	void CombineAABBs(const AABB2D& combinableAABB);
	static AABB2D GetCombinedAABB(const AABB2D& firstCombinableAABB, const AABB2D& secondCombinableAABB);
	static bool AreAABBsIntersecting(const AABB2D& firstAABB, const AABB2D& secondAABB);

public:
	Vector2D minimums;
	Vector2D maximums;
};



class Rotator2D
{
public:
	Rotator2D();
	Rotator2D(float angleInDegrees);

	void SetIdentityRotator2D();
	void SetRotator2D(float angleInDegrees);

	float GetAngle() const;

public:
	float m_SinAngle;
	float m_CosAngle;
};



class Transform2D
{
public:
	Transform2D();
	Transform2D(const Vector2D& transformPosition, float transformRotation);

	void SetIdentityTransform2D();
	void SetTransform2D(const Vector2D& transformPosition, float transformRotation);

public:
	Vector2D m_Position;
	Rotator2D m_Rotation;
};



float SquareRootOfValue(float value);
float FloorOfValue(float value);



Vector2D Multiply(const Transform2D& transform2D, const Vector2D& vector2D);
Vector2D MultiplyTranspose(const Transform2D& transform2D, const Vector2D& vector2D);

Vector2D Multiply(const Rotator2D& rotation2D, const Vector2D& vector2D);
Vector2D MultiplyTranspose(const Rotator2D& rotation2D, const Vector2D& vector2D);

Transform2D Multiply(const Transform2D& firstTransform2D, const Transform2D& secondTransform2D);
Transform2D MultiplyTranspose(const Transform2D& firstTransform2D, const Transform2D& secondTransform2D);



inline Vector2D Multiply(const Matrix2x2& matrix2x2, const Vector2D& vector2D)
{
	Vector2D resultantVector2D;
	resultantVector2D.X = vector2D.X * matrix2x2.m_Matrix2x2[0] + vector2D.Y * matrix2x2.m_Matrix2x2[2];
	resultantVector2D.Y = vector2D.X * matrix2x2.m_Matrix2x2[1] + vector2D.Y * matrix2x2.m_Matrix2x2[3];

	return resultantVector2D;
}



inline Vector2D MultiplyTranspose(const Matrix2x2& matrix2x2, const Vector2D& vector2D)
{
	Matrix2x2 transposedMatrix2x2 = matrix2x2.GetTranspose();

	return Multiply(transposedMatrix2x2, vector2D);
}



inline Matrix2x2 Multiply(const Matrix2x2& firstMatrix2x2, const Matrix2x2& secondMatrix2x2)
{
	Matrix2x2 resultantMatrix2x2 = Matrix2x2::IdentityMatrix2x2();

	resultantMatrix2x2.m_Matrix2x2[0] = (firstMatrix2x2.m_Matrix2x2[0] * secondMatrix2x2.m_Matrix2x2[0]) + (firstMatrix2x2.m_Matrix2x2[1] * secondMatrix2x2.m_Matrix2x2[2]);
	resultantMatrix2x2.m_Matrix2x2[1] = (firstMatrix2x2.m_Matrix2x2[0] * secondMatrix2x2.m_Matrix2x2[1]) + (firstMatrix2x2.m_Matrix2x2[1] * secondMatrix2x2.m_Matrix2x2[3]);

	resultantMatrix2x2.m_Matrix2x2[2] = (firstMatrix2x2.m_Matrix2x2[2] * secondMatrix2x2.m_Matrix2x2[0]) + (firstMatrix2x2.m_Matrix2x2[3] * secondMatrix2x2.m_Matrix2x2[2]);
	resultantMatrix2x2.m_Matrix2x2[3] = (firstMatrix2x2.m_Matrix2x2[2] * secondMatrix2x2.m_Matrix2x2[1]) + (firstMatrix2x2.m_Matrix2x2[3] * secondMatrix2x2.m_Matrix2x2[3]);

	return resultantMatrix2x2;
}



inline Matrix2x2 MultiplyTranspose(const Matrix2x2& firstMatrix2x2, const Matrix2x2& secondMatrix2x2)
{
	Matrix2x2 transposedFirstMatrix2x2 = firstMatrix2x2.GetTranspose();

	return Multiply(transposedFirstMatrix2x2, secondMatrix2x2);
}



inline Rotator2D Multiply(const Rotator2D& firstRotator2D, const Rotator2D& secondRotator2D)
{
	Rotator2D resultantRotator2D;
	resultantRotator2D.m_SinAngle = (firstRotator2D.m_SinAngle * secondRotator2D.m_CosAngle) + (firstRotator2D.m_CosAngle * secondRotator2D.m_SinAngle);
	resultantRotator2D.m_CosAngle = (firstRotator2D.m_CosAngle * secondRotator2D.m_CosAngle) - (firstRotator2D.m_SinAngle * secondRotator2D.m_SinAngle);

	return resultantRotator2D;
}



inline Rotator2D MultiplyTranspose(const Rotator2D& firstRotator2D, const Rotator2D& secondRotator2D)
{
	Rotator2D resultantRotator2D;
	resultantRotator2D.m_SinAngle = (firstRotator2D.m_CosAngle * secondRotator2D.m_SinAngle) - (firstRotator2D.m_SinAngle * secondRotator2D.m_CosAngle);
	resultantRotator2D.m_CosAngle = (firstRotator2D.m_CosAngle * secondRotator2D.m_CosAngle) + (firstRotator2D.m_SinAngle * secondRotator2D.m_SinAngle);

	return resultantRotator2D;
}



inline Transform2D Multiply(const Transform2D& firstTransform2D, const Transform2D& secondTransform2D)
{
	Transform2D resultantTransform2D;
	resultantTransform2D.m_Rotation = Multiply(firstTransform2D.m_Rotation, secondTransform2D.m_Rotation);
	resultantTransform2D.m_Position = Multiply(firstTransform2D.m_Rotation, secondTransform2D.m_Position) + firstTransform2D.m_Position;

	return resultantTransform2D;
}



inline Transform2D MultiplyTranspose(const Transform2D& firstTransform2D, const Transform2D& secondTransform2D)
{
	Transform2D resultantTransform2D;
	resultantTransform2D.m_Rotation = MultiplyTranspose(firstTransform2D.m_Rotation, secondTransform2D.m_Rotation);
	resultantTransform2D.m_Position = MultiplyTranspose(firstTransform2D.m_Rotation, secondTransform2D.m_Position - firstTransform2D.m_Position);

	return resultantTransform2D;
}



template <typename data_type>
inline data_type GetMinimum(data_type firstValue, data_type secondValue)
{
	return (firstValue < secondValue ? firstValue : secondValue);
}



template <typename data_type>
inline data_type GetMaximum(data_type firstValue, data_type secondValue)
{
	return (firstValue > secondValue ? firstValue : secondValue);
}



inline Vector2D GetMinimum(const Vector2D& firstVector2D, const Vector2D& secondVector2D)
{
	return Vector2D(GetMinimum(firstVector2D.X, secondVector2D.X), GetMinimum(firstVector2D.Y, secondVector2D.Y));
}



inline Vector2D GetMaximum(const Vector2D& firstVector2D, const Vector2D& secondVector2D)
{
	return Vector2D(GetMaximum(firstVector2D.X, secondVector2D.X), GetMaximum(firstVector2D.Y, secondVector2D.Y));
}



template <typename data_type>
data_type ClampValue(data_type inputValue, data_type clampMinimum, data_type clampMaximum)
{
	if (inputValue < clampMinimum)
	{
		return clampMinimum;
	}
	else if (inputValue > clampMaximum)
	{
		return clampMaximum;
	}
	else
	{
		return inputValue;
	}
}



template <typename data_type>
inline data_type Absolute(data_type value)
{
	if (value > data_type(0))
	{
		return value;
	}
	else
	{
		return -value;
	}
}



inline Vector2D Absolute(const Vector2D& vector2D)
{
	return Vector2D(Absolute(vector2D.X), Absolute(vector2D.Y));
}



struct RaycastInput
{
	Vector2D m_StartingPosition;
	Vector2D m_EndingPosition;
	float m_MaximumImpactFraction;
};



struct RaycastResult
{
	Vector2D m_ImpactNormal;
	float m_ImpactFraction;
};