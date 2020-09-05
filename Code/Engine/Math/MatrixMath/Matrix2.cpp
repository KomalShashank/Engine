#include "Engine/Math/MatrixMath/Matrix2.hpp"



Matrix2::Matrix2()
{

}



Matrix2::Matrix2(const float* matrix2)
{
	m_Matrix2[0] = matrix2[0];
	m_Matrix2[1] = matrix2[1];

	m_Matrix2[2] = matrix2[2];
	m_Matrix2[3] = matrix2[3];
}



Matrix2::Matrix2(const Matrix2& copyableMatrix2)
{
	m_Matrix2[0] = copyableMatrix2.m_Matrix2[0];
	m_Matrix2[1] = copyableMatrix2.m_Matrix2[1];

	m_Matrix2[2] = copyableMatrix2.m_Matrix2[2];
	m_Matrix2[3] = copyableMatrix2.m_Matrix2[3];
}



Matrix2 Matrix2::IdentityMatrix2()
{
	Matrix2 identity;

	identity.m_Matrix2[0] = 1.0f;
	identity.m_Matrix2[1] = 0.0f;

	identity.m_Matrix2[2] = 0.0f;
	identity.m_Matrix2[3] = 1.0f;

	return identity;
}



float Matrix2::GetDeterminant() const
{
	float A_11 = m_Matrix2[0];
	float A_12 = m_Matrix2[1];

	float A_21 = m_Matrix2[2];
	float A_22 = m_Matrix2[3];

	return (A_11 * A_22) - (A_12 * A_21);
}



float Matrix2::GetTrace() const
{
	return (m_Matrix2[0] + m_Matrix2[3]);
}



bool Matrix2::HasInverse(Matrix2& outputInverseMatrix2) const
{
	float determinant = GetDeterminant();

	if (determinant == 0.0f)
	{
		return false;
	}

	float oneOverDeterminant = 1.0f / determinant;

	outputInverseMatrix2.m_Matrix2[0] = oneOverDeterminant * m_Matrix2[3];
	outputInverseMatrix2.m_Matrix2[1] = oneOverDeterminant * -m_Matrix2[1];

	outputInverseMatrix2.m_Matrix2[2] = oneOverDeterminant * -m_Matrix2[2];
	outputInverseMatrix2.m_Matrix2[3] = oneOverDeterminant * m_Matrix2[0];

	return true;
}



void Matrix2::GetTranspose(Matrix2& outputTransposeMatrix2) const
{
	outputTransposeMatrix2.m_Matrix2[0] = m_Matrix2[0];
	outputTransposeMatrix2.m_Matrix2[1] = m_Matrix2[2];

	outputTransposeMatrix2.m_Matrix2[2] = m_Matrix2[1];
	outputTransposeMatrix2.m_Matrix2[3] = m_Matrix2[3];
}



void Matrix2::Multiply(const Matrix2& multipliableMatrix2, Matrix2& outputResultantMatrix2) const
{
	outputResultantMatrix2.m_Matrix2[0] = (m_Matrix2[0] * multipliableMatrix2.m_Matrix2[0]) + (m_Matrix2[1] * multipliableMatrix2.m_Matrix2[2]);
	outputResultantMatrix2.m_Matrix2[1] = (m_Matrix2[0] * multipliableMatrix2.m_Matrix2[1]) + (m_Matrix2[1] * multipliableMatrix2.m_Matrix2[3]);

	outputResultantMatrix2.m_Matrix2[2] = (m_Matrix2[2] * multipliableMatrix2.m_Matrix2[0]) + (m_Matrix2[3] * multipliableMatrix2.m_Matrix2[2]);
	outputResultantMatrix2.m_Matrix2[3] = (m_Matrix2[2] * multipliableMatrix2.m_Matrix2[1]) + (m_Matrix2[3] * multipliableMatrix2.m_Matrix2[3]);
}



void Matrix2::SetTranslationMatrix2(float translationPosition, Matrix2& outputResultantMatrix2)
{
	outputResultantMatrix2 = Matrix2::IdentityMatrix2();
	outputResultantMatrix2.m_Matrix2[2] = translationPosition;
}



Matrix2 Matrix2::operator+(const Matrix2& addableMatrix2) const
{
	Matrix2 resultantMatrix2;

	resultantMatrix2.m_Matrix2[0] = m_Matrix2[0] + addableMatrix2.m_Matrix2[0];
	resultantMatrix2.m_Matrix2[1] = m_Matrix2[1] + addableMatrix2.m_Matrix2[1];

	resultantMatrix2.m_Matrix2[2] = m_Matrix2[2] + addableMatrix2.m_Matrix2[2];
	resultantMatrix2.m_Matrix2[3] = m_Matrix2[3] + addableMatrix2.m_Matrix2[3];

	return resultantMatrix2;
}



Matrix2 Matrix2::operator-(const Matrix2& subtractableMatrix2) const
{
	Matrix2 resultantMatrix2;

	resultantMatrix2.m_Matrix2[0] = m_Matrix2[0] - subtractableMatrix2.m_Matrix2[0];
	resultantMatrix2.m_Matrix2[1] = m_Matrix2[1] - subtractableMatrix2.m_Matrix2[1];

	resultantMatrix2.m_Matrix2[2] = m_Matrix2[2] - subtractableMatrix2.m_Matrix2[2];
	resultantMatrix2.m_Matrix2[3] = m_Matrix2[3] - subtractableMatrix2.m_Matrix2[3];

	return resultantMatrix2;
}



Matrix2 Matrix2::operator*(float multipliableValue) const
{
	Matrix2 resultantMatrix2;

	resultantMatrix2.m_Matrix2[0] = m_Matrix2[0] * multipliableValue;
	resultantMatrix2.m_Matrix2[1] = m_Matrix2[1] * multipliableValue;

	resultantMatrix2.m_Matrix2[2] = m_Matrix2[2] * multipliableValue;
	resultantMatrix2.m_Matrix2[3] = m_Matrix2[3] * multipliableValue;

	return resultantMatrix2;
}



Matrix2 Matrix2::operator/(float dividableValue) const
{
	Matrix2 resultantMatrix2;

	resultantMatrix2.m_Matrix2[0] = m_Matrix2[0] / dividableValue;
	resultantMatrix2.m_Matrix2[1] = m_Matrix2[1] / dividableValue;

	resultantMatrix2.m_Matrix2[2] = m_Matrix2[2] / dividableValue;
	resultantMatrix2.m_Matrix2[3] = m_Matrix2[3] / dividableValue;

	return resultantMatrix2;
}



void Matrix2::operator+=(const Matrix2& addableMatrix2)
{
	m_Matrix2[0] += addableMatrix2.m_Matrix2[0];
	m_Matrix2[1] += addableMatrix2.m_Matrix2[1];

	m_Matrix2[2] += addableMatrix2.m_Matrix2[2];
	m_Matrix2[3] += addableMatrix2.m_Matrix2[3];
}



void Matrix2::operator-=(const Matrix2& subtractableMatrix2)
{
	m_Matrix2[0] -= subtractableMatrix2.m_Matrix2[0];
	m_Matrix2[1] -= subtractableMatrix2.m_Matrix2[1];

	m_Matrix2[2] -= subtractableMatrix2.m_Matrix2[2];
	m_Matrix2[3] -= subtractableMatrix2.m_Matrix2[3];
}



void Matrix2::operator*=(float multipliableValue)
{
	m_Matrix2[0] *= multipliableValue;
	m_Matrix2[1] *= multipliableValue;

	m_Matrix2[2] *= multipliableValue;
	m_Matrix2[3] *= multipliableValue;
}



void Matrix2::operator/=(float dividableValue)
{
	m_Matrix2[0] /= dividableValue;
	m_Matrix2[1] /= dividableValue;

	m_Matrix2[2] /= dividableValue;
	m_Matrix2[3] /= dividableValue;
}



bool Matrix2::operator==(const Matrix2& comparableMatrix2) const
{
	return (m_Matrix2[0] == comparableMatrix2.m_Matrix2[0] &&
			m_Matrix2[1] == comparableMatrix2.m_Matrix2[1] &&

			m_Matrix2[2] == comparableMatrix2.m_Matrix2[2] &&
			m_Matrix2[3] == comparableMatrix2.m_Matrix2[3]);
}



bool Matrix2::operator!=(const Matrix2& comparableMatrix2) const
{
	return (m_Matrix2[0] != comparableMatrix2.m_Matrix2[0] ||
			m_Matrix2[1] != comparableMatrix2.m_Matrix2[1] ||

			m_Matrix2[2] != comparableMatrix2.m_Matrix2[2] ||
			m_Matrix2[3] != comparableMatrix2.m_Matrix2[3]);
}



bool Matrix2::IsSymmetric() const
{
	Matrix2 transposeMatrix2;
	GetTranspose(transposeMatrix2);

	if ((*this) == transposeMatrix2)
	{
		return true;
	}

	return false;
}



bool Matrix2::IsSkewSymmetric() const
{
	Matrix2 transposeMatrix2;
	GetTranspose(transposeMatrix2);

	Matrix2 negatedTransposeMatrix2 = transposeMatrix2 * -1.0f;

	if ((*this) == negatedTransposeMatrix2)
	{
		return true;
	}

	return false;
}



bool Matrix2::IsOrthogonal() const
{
	Matrix2 inverseMatrix2;

	if (HasInverse(inverseMatrix2))
	{
		Matrix2 transposeMatrix2;
		GetTranspose(transposeMatrix2);

		if (inverseMatrix2 == transposeMatrix2)
		{
			return true;
		}
	}

	return false;
}