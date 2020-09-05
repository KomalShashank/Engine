#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"



class Matrix3
{
public:
	Matrix3();
	Matrix3(const float* matrix3);
	Matrix3(const Matrix3& copyableMatrix3);

	static Matrix3 IdentityMatrix3();

	float GetDeterminant() const;
	float GetTrace() const;

	bool HasInverse(Matrix3& outputInverseMatrix3) const;
	void GetTranspose(Matrix3& outputTransposeMatrix3) const;

	void Multiply(const Matrix3& multipliableMatrix3, Matrix3& outputResultantMatrix3) const;

	static void SetTranslationMatrix3(const Vector2& translationPosition, Matrix3& outputResultantMatrix3);
	static void SetRotationMatrix3(float angleOfRotationInDegrees, Matrix3& outputResultantMatrix3);

	Matrix3 operator+(const Matrix3& addableMatrix3) const;
	Matrix3 operator-(const Matrix3& subtractableMatrix3) const;
	Matrix3 operator*(float multipliableValue) const;
	Matrix3 operator/(float dividableValue) const;

	void operator+=(const Matrix3& addableMatrix3);
	void operator-=(const Matrix3& subtractableMatrix3);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Matrix3& comparableMatrix3) const;
	bool operator!=(const Matrix3& comparableMatrix3) const;

	void GetBasisVectors(Vector2& firstRow, Vector2& secondRow, Vector2& thirdRow) const;
	void SetBasisVectors(const Vector2& firstRow, const Vector2& secondRow, const Vector2& thirdRow);

	bool IsSymmetric() const;
	bool IsSkewSymmetric() const;

	bool IsOrthogonal() const;

public:
	float m_Matrix3[9];
};