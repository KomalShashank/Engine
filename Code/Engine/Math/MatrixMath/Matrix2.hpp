#pragma once



class Matrix2
{
public:
	Matrix2();
	Matrix2(const float* matrix2);
	Matrix2(const Matrix2& copyableMatrix2);

	static Matrix2 IdentityMatrix2();

	float GetDeterminant() const;
	float GetTrace() const;

	bool HasInverse(Matrix2& outputInverseMatrix2) const;
	void GetTranspose(Matrix2& outputTransposeMatrix2) const;

	void Multiply(const Matrix2& multipliableMatrix2, Matrix2& outputResultantMatrix2) const;

	static void SetTranslationMatrix2(float translationPosition, Matrix2& outputResultantMatrix2);

	Matrix2 operator+(const Matrix2& addableMatrix2) const;
	Matrix2 operator-(const Matrix2& subtractableMatrix2) const;
	Matrix2 operator*(float multipliableValue) const;
	Matrix2 operator/(float dividableValue) const;

	void operator+=(const Matrix2& addableMatrix2);
	void operator-=(const Matrix2& subtractableMatrix2);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Matrix2& comparableMatrix2) const;
	bool operator!=(const Matrix2& comparableMatrix2) const;

	bool IsSymmetric() const;
	bool IsSkewSymmetric() const;

	bool IsOrthogonal() const;

public:
	float m_Matrix2[4];
};