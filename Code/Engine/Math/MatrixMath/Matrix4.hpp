#pragma once

#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/EulerAngles/EulerAngles.hpp"



class Matrix4
{
public:
	Matrix4();
	Matrix4(const float* matrix4);
	Matrix4(const Matrix4& copyableMatrix4);

	static Matrix4 IdentityMatrix4();

	float GetDeterminant() const;
	float GetTrace() const;

	bool HasInverse(Matrix4& outputInverseMatrix4) const;
	Matrix4 GetTranspose() const;

	static Matrix4 Multiply(const Matrix4& firstMatrix4, const Matrix4& secondMatrix4);
	Vector3 TransformVector3(const Vector3& transformableVector3, float tComponent) const;

	static Matrix4 SetTranslationMatrix4(const Vector3& translationPosition);
	static Matrix4 InterpolateMatrices4(const Matrix4& firstMatrix4, const Matrix4& secondMatrix4, float interpolationFactor);

	Matrix4 operator+(const Matrix4& addableMatrix4) const;
	Matrix4 operator-(const Matrix4& subtractableMatrix4) const;
	Matrix4 operator*(float multipliableValue) const;
	Matrix4 operator/(float dividableValue) const;

	void operator+=(const Matrix4& addableMatrix4);
	void operator-=(const Matrix4& subtractableMatrix4);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Matrix4& comparableMatrix4) const;
	bool operator!=(const Matrix4& comparableMatrix4) const;

	Matrix4 operator*(const Matrix4& multipliableMatrix4) const;

	void GetBasisVectors(Vector3& firstRow, Vector3& secondRow, Vector3& thirdRow, Vector3& fourthRow) const;
	void SetBasisVectors(const Vector3& firstRow, const Vector3& secondRow, const Vector3& thirdRow, const Vector3& fourthRow);

	bool IsSymmetric() const;
	bool IsSkewSymmetric() const;

	bool IsOrthogonal() const;

public:
	float m_Matrix4[16];
};