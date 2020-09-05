#include "Engine/Math/MatrixMath/Matrix3.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Matrix3::Matrix3()
{

}



Matrix3::Matrix3(const float* matrix3)
{
	m_Matrix3[0] = matrix3[0];
	m_Matrix3[1] = matrix3[1];
	m_Matrix3[2] = matrix3[2];

	m_Matrix3[3] = matrix3[3];
	m_Matrix3[4] = matrix3[4];
	m_Matrix3[5] = matrix3[5];

	m_Matrix3[6] = matrix3[6];
	m_Matrix3[7] = matrix3[7];
	m_Matrix3[8] = matrix3[8];
}



Matrix3::Matrix3(const Matrix3& copyableMatrix3)
{
	m_Matrix3[0] = copyableMatrix3.m_Matrix3[0];
	m_Matrix3[1] = copyableMatrix3.m_Matrix3[1];
	m_Matrix3[2] = copyableMatrix3.m_Matrix3[2];

	m_Matrix3[3] = copyableMatrix3.m_Matrix3[3];
	m_Matrix3[4] = copyableMatrix3.m_Matrix3[4];
	m_Matrix3[5] = copyableMatrix3.m_Matrix3[5];

	m_Matrix3[6] = copyableMatrix3.m_Matrix3[6];
	m_Matrix3[7] = copyableMatrix3.m_Matrix3[7];
	m_Matrix3[8] = copyableMatrix3.m_Matrix3[8];
}



Matrix3 Matrix3::IdentityMatrix3()
{
	Matrix3 identity;

	identity.m_Matrix3[0] = 1.0f;
	identity.m_Matrix3[1] = 0.0f;
	identity.m_Matrix3[2] = 0.0f;
							
	identity.m_Matrix3[3] = 0.0f;
	identity.m_Matrix3[4] = 1.0f;
	identity.m_Matrix3[5] = 0.0f;
							
	identity.m_Matrix3[6] = 0.0f;
	identity.m_Matrix3[7] = 0.0f;
	identity.m_Matrix3[8] = 1.0f;

	return identity;
}



float Matrix3::GetDeterminant() const
{
	float determinant = 0.0f;

	const float& A_11 = m_Matrix3[0];
	const float& A_12 = m_Matrix3[1];
	const float& A_13 = m_Matrix3[2];

	const float& A_21 = m_Matrix3[3];
	const float& A_22 = m_Matrix3[4];
	const float& A_23 = m_Matrix3[5];

	const float& A_31 = m_Matrix3[6];
	const float& A_32 = m_Matrix3[7];
	const float& A_33 = m_Matrix3[8];

	determinant += A_11 * A_22 * A_33;
	determinant += A_21 * A_32 * A_13;
	determinant += A_31 * A_12 * A_23;

	determinant -= A_11 * A_32 * A_23;
	determinant -= A_31 * A_22 * A_13;
	determinant -= A_21 * A_12 * A_33;

	return determinant;
}



float Matrix3::GetTrace() const
{
	return (m_Matrix3[0] + m_Matrix3[4] + m_Matrix3[8]);
}



bool Matrix3::HasInverse(Matrix3& outputInverseMatrix3) const
{
	float determinant = GetDeterminant();

	if (determinant == 0.0f)
	{
		return false;
	}

	const float& A_11 = m_Matrix3[0];
	const float& A_12 = m_Matrix3[1];
	const float& A_13 = m_Matrix3[2];

	const float& A_21 = m_Matrix3[3];
	const float& A_22 = m_Matrix3[4];
	const float& A_23 = m_Matrix3[5];

	const float& A_31 = m_Matrix3[6];
	const float& A_32 = m_Matrix3[7];
	const float& A_33 = m_Matrix3[8];

	float oneOverDeterminant = 1.0f / determinant;

	outputInverseMatrix3.m_Matrix3[0] = ((A_22 * A_33) - (A_23 * A_32)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[1] = ((A_13 * A_32) - (A_12 * A_33)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[2] = ((A_12 * A_23) - (A_13 * A_22)) * oneOverDeterminant;

	outputInverseMatrix3.m_Matrix3[3] = ((A_23 * A_31) - (A_21 * A_33)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[4] = ((A_11 * A_33) - (A_13 * A_31)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[5] = ((A_13 * A_21) - (A_11 * A_23)) * oneOverDeterminant;

	outputInverseMatrix3.m_Matrix3[6] = ((A_21 * A_32) - (A_22 * A_31)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[7] = ((A_12 * A_31) - (A_11 * A_32)) * oneOverDeterminant;
	outputInverseMatrix3.m_Matrix3[8] = ((A_11 * A_22) - (A_12 * A_21)) * oneOverDeterminant;

	return true;
}



void Matrix3::GetTranspose(Matrix3& outputTransposeMatrix3) const
{
	outputTransposeMatrix3.m_Matrix3[0] = m_Matrix3[0];
	outputTransposeMatrix3.m_Matrix3[1] = m_Matrix3[3];
	outputTransposeMatrix3.m_Matrix3[2] = m_Matrix3[6];

	outputTransposeMatrix3.m_Matrix3[3] = m_Matrix3[1];
	outputTransposeMatrix3.m_Matrix3[4] = m_Matrix3[4];
	outputTransposeMatrix3.m_Matrix3[5] = m_Matrix3[7];

	outputTransposeMatrix3.m_Matrix3[6] = m_Matrix3[2];
	outputTransposeMatrix3.m_Matrix3[7] = m_Matrix3[5];
	outputTransposeMatrix3.m_Matrix3[8] = m_Matrix3[8];
}



void Matrix3::Multiply(const Matrix3& multipliableMatrix3, Matrix3& outputResultantMatrix3) const
{
	outputResultantMatrix3.m_Matrix3[0] = (m_Matrix3[0] * multipliableMatrix3.m_Matrix3[0]) + (m_Matrix3[1] * multipliableMatrix3.m_Matrix3[3]) + (m_Matrix3[2] * multipliableMatrix3.m_Matrix3[6]);
	outputResultantMatrix3.m_Matrix3[1] = (m_Matrix3[0] * multipliableMatrix3.m_Matrix3[1]) + (m_Matrix3[1] * multipliableMatrix3.m_Matrix3[4]) + (m_Matrix3[2] * multipliableMatrix3.m_Matrix3[7]);
	outputResultantMatrix3.m_Matrix3[2] = (m_Matrix3[0] * multipliableMatrix3.m_Matrix3[2]) + (m_Matrix3[1] * multipliableMatrix3.m_Matrix3[5]) + (m_Matrix3[2] * multipliableMatrix3.m_Matrix3[8]);

	outputResultantMatrix3.m_Matrix3[3] = (m_Matrix3[3] * multipliableMatrix3.m_Matrix3[0]) + (m_Matrix3[4] * multipliableMatrix3.m_Matrix3[3]) + (m_Matrix3[5] * multipliableMatrix3.m_Matrix3[6]);
	outputResultantMatrix3.m_Matrix3[4] = (m_Matrix3[3] * multipliableMatrix3.m_Matrix3[1]) + (m_Matrix3[4] * multipliableMatrix3.m_Matrix3[4]) + (m_Matrix3[5] * multipliableMatrix3.m_Matrix3[7]);
	outputResultantMatrix3.m_Matrix3[5] = (m_Matrix3[3] * multipliableMatrix3.m_Matrix3[2]) + (m_Matrix3[4] * multipliableMatrix3.m_Matrix3[5]) + (m_Matrix3[5] * multipliableMatrix3.m_Matrix3[8]);

	outputResultantMatrix3.m_Matrix3[6] = (m_Matrix3[6] * multipliableMatrix3.m_Matrix3[0]) + (m_Matrix3[7] * multipliableMatrix3.m_Matrix3[3]) + (m_Matrix3[8] * multipliableMatrix3.m_Matrix3[6]);
	outputResultantMatrix3.m_Matrix3[7] = (m_Matrix3[6] * multipliableMatrix3.m_Matrix3[1]) + (m_Matrix3[7] * multipliableMatrix3.m_Matrix3[4]) + (m_Matrix3[8] * multipliableMatrix3.m_Matrix3[7]);
	outputResultantMatrix3.m_Matrix3[8] = (m_Matrix3[6] * multipliableMatrix3.m_Matrix3[2]) + (m_Matrix3[7] * multipliableMatrix3.m_Matrix3[5]) + (m_Matrix3[8] * multipliableMatrix3.m_Matrix3[8]);
}



void Matrix3::SetTranslationMatrix3(const Vector2& translationPosition, Matrix3& outputResultantMatrix3)
{
	outputResultantMatrix3 = Matrix3::IdentityMatrix3();

	outputResultantMatrix3.m_Matrix3[6] = translationPosition.X;
	outputResultantMatrix3.m_Matrix3[7] = translationPosition.Y;
}



void Matrix3::SetRotationMatrix3(float angleOfRotationInDegrees, Matrix3& outputResultantMatrix3)
{
	outputResultantMatrix3 = Matrix3::IdentityMatrix3();

	float cosTheta = CosineOfDegrees(angleOfRotationInDegrees);
	float sinTheta = SineOfDegrees(angleOfRotationInDegrees);

	outputResultantMatrix3.m_Matrix3[0] = cosTheta;
	outputResultantMatrix3.m_Matrix3[1] = -sinTheta;

	outputResultantMatrix3.m_Matrix3[3] = sinTheta;
	outputResultantMatrix3.m_Matrix3[4] = cosTheta;
}



Matrix3 Matrix3::operator+(const Matrix3& addableMatrix3) const
{
	Matrix3 resultantMatrix3;

	resultantMatrix3.m_Matrix3[0] = m_Matrix3[0] + addableMatrix3.m_Matrix3[0];
	resultantMatrix3.m_Matrix3[1] = m_Matrix3[1] + addableMatrix3.m_Matrix3[1];
	resultantMatrix3.m_Matrix3[2] = m_Matrix3[2] + addableMatrix3.m_Matrix3[2];

	resultantMatrix3.m_Matrix3[3] = m_Matrix3[3] + addableMatrix3.m_Matrix3[3];
	resultantMatrix3.m_Matrix3[4] = m_Matrix3[4] + addableMatrix3.m_Matrix3[4];
	resultantMatrix3.m_Matrix3[5] = m_Matrix3[5] + addableMatrix3.m_Matrix3[5];

	resultantMatrix3.m_Matrix3[6] = m_Matrix3[6] + addableMatrix3.m_Matrix3[6];
	resultantMatrix3.m_Matrix3[7] = m_Matrix3[7] + addableMatrix3.m_Matrix3[7];
	resultantMatrix3.m_Matrix3[8] = m_Matrix3[8] + addableMatrix3.m_Matrix3[8];

	return resultantMatrix3;
}



Matrix3 Matrix3::operator-(const Matrix3& subtractableMatrix3) const
{
	Matrix3 resultantMatrix3;

	resultantMatrix3.m_Matrix3[0] = m_Matrix3[0] - subtractableMatrix3.m_Matrix3[0];
	resultantMatrix3.m_Matrix3[1] = m_Matrix3[1] - subtractableMatrix3.m_Matrix3[1];
	resultantMatrix3.m_Matrix3[2] = m_Matrix3[2] - subtractableMatrix3.m_Matrix3[2];

	resultantMatrix3.m_Matrix3[3] = m_Matrix3[3] - subtractableMatrix3.m_Matrix3[3];
	resultantMatrix3.m_Matrix3[4] = m_Matrix3[4] - subtractableMatrix3.m_Matrix3[4];
	resultantMatrix3.m_Matrix3[5] = m_Matrix3[5] - subtractableMatrix3.m_Matrix3[5];

	resultantMatrix3.m_Matrix3[6] = m_Matrix3[6] - subtractableMatrix3.m_Matrix3[6];
	resultantMatrix3.m_Matrix3[7] = m_Matrix3[7] - subtractableMatrix3.m_Matrix3[7];
	resultantMatrix3.m_Matrix3[8] = m_Matrix3[8] - subtractableMatrix3.m_Matrix3[8];

	return resultantMatrix3;
}



Matrix3 Matrix3::operator*(float multipliableValue) const
{
	Matrix3 resultantMatrix3;

	resultantMatrix3.m_Matrix3[0] = m_Matrix3[0] * multipliableValue;
	resultantMatrix3.m_Matrix3[1] = m_Matrix3[1] * multipliableValue;
	resultantMatrix3.m_Matrix3[2] = m_Matrix3[2] * multipliableValue;

	resultantMatrix3.m_Matrix3[3] = m_Matrix3[3] * multipliableValue;
	resultantMatrix3.m_Matrix3[4] = m_Matrix3[4] * multipliableValue;
	resultantMatrix3.m_Matrix3[5] = m_Matrix3[5] * multipliableValue;

	resultantMatrix3.m_Matrix3[6] = m_Matrix3[6] * multipliableValue;
	resultantMatrix3.m_Matrix3[7] = m_Matrix3[7] * multipliableValue;
	resultantMatrix3.m_Matrix3[8] = m_Matrix3[8] * multipliableValue;

	return resultantMatrix3;
}



Matrix3 Matrix3::operator/(float dividableValue) const
{
	Matrix3 resultantMatrix3;

	resultantMatrix3.m_Matrix3[0] = m_Matrix3[0] / dividableValue;
	resultantMatrix3.m_Matrix3[1] = m_Matrix3[1] / dividableValue;
	resultantMatrix3.m_Matrix3[2] = m_Matrix3[2] / dividableValue;

	resultantMatrix3.m_Matrix3[3] = m_Matrix3[3] / dividableValue;
	resultantMatrix3.m_Matrix3[4] = m_Matrix3[4] / dividableValue;
	resultantMatrix3.m_Matrix3[5] = m_Matrix3[5] / dividableValue;

	resultantMatrix3.m_Matrix3[6] = m_Matrix3[6] / dividableValue;
	resultantMatrix3.m_Matrix3[7] = m_Matrix3[7] / dividableValue;
	resultantMatrix3.m_Matrix3[8] = m_Matrix3[8] / dividableValue;

	return resultantMatrix3;
}



void Matrix3::operator+=(const Matrix3& addableMatrix3)
{
	m_Matrix3[0] += addableMatrix3.m_Matrix3[0];
	m_Matrix3[1] += addableMatrix3.m_Matrix3[1];
	m_Matrix3[2] += addableMatrix3.m_Matrix3[2];

	m_Matrix3[3] += addableMatrix3.m_Matrix3[3];
	m_Matrix3[4] += addableMatrix3.m_Matrix3[4];
	m_Matrix3[5] += addableMatrix3.m_Matrix3[5];

	m_Matrix3[6] += addableMatrix3.m_Matrix3[6];
	m_Matrix3[7] += addableMatrix3.m_Matrix3[7];
	m_Matrix3[8] += addableMatrix3.m_Matrix3[8];
}



void Matrix3::operator-=(const Matrix3& subtractableMatrix3)
{
	m_Matrix3[0] -= subtractableMatrix3.m_Matrix3[0];
	m_Matrix3[1] -= subtractableMatrix3.m_Matrix3[1];
	m_Matrix3[2] -= subtractableMatrix3.m_Matrix3[2];

	m_Matrix3[3] -= subtractableMatrix3.m_Matrix3[3];
	m_Matrix3[4] -= subtractableMatrix3.m_Matrix3[4];
	m_Matrix3[5] -= subtractableMatrix3.m_Matrix3[5];

	m_Matrix3[6] -= subtractableMatrix3.m_Matrix3[6];
	m_Matrix3[7] -= subtractableMatrix3.m_Matrix3[7];
	m_Matrix3[8] -= subtractableMatrix3.m_Matrix3[8];
}



void Matrix3::operator*=(float multipliableValue)
{
	m_Matrix3[0] *= multipliableValue;
	m_Matrix3[1] *= multipliableValue;
	m_Matrix3[2] *= multipliableValue;

	m_Matrix3[3] *= multipliableValue;
	m_Matrix3[4] *= multipliableValue;
	m_Matrix3[5] *= multipliableValue;

	m_Matrix3[6] *= multipliableValue;
	m_Matrix3[7] *= multipliableValue;
	m_Matrix3[8] *= multipliableValue;
}



void Matrix3::operator/=(float dividableValue)
{
	m_Matrix3[0] /= dividableValue;
	m_Matrix3[1] /= dividableValue;
	m_Matrix3[2] /= dividableValue;

	m_Matrix3[3] /= dividableValue;
	m_Matrix3[4] /= dividableValue;
	m_Matrix3[5] /= dividableValue;

	m_Matrix3[6] /= dividableValue;
	m_Matrix3[7] /= dividableValue;
	m_Matrix3[8] /= dividableValue;
}



bool Matrix3::operator==(const Matrix3& comparableMatrix3) const
{
	return (m_Matrix3[0] == comparableMatrix3.m_Matrix3[0] &&
			m_Matrix3[1] == comparableMatrix3.m_Matrix3[1] &&
			m_Matrix3[2] == comparableMatrix3.m_Matrix3[2] &&

			m_Matrix3[3] == comparableMatrix3.m_Matrix3[3] &&
			m_Matrix3[4] == comparableMatrix3.m_Matrix3[4] &&
			m_Matrix3[5] == comparableMatrix3.m_Matrix3[5] &&

			m_Matrix3[6] == comparableMatrix3.m_Matrix3[6] &&
			m_Matrix3[7] == comparableMatrix3.m_Matrix3[7] &&
			m_Matrix3[8] == comparableMatrix3.m_Matrix3[8]);
}



bool Matrix3::operator!=(const Matrix3& comparableMatrix3) const
{
	return (m_Matrix3[0] != comparableMatrix3.m_Matrix3[0] ||
			m_Matrix3[1] != comparableMatrix3.m_Matrix3[1] ||
			m_Matrix3[2] != comparableMatrix3.m_Matrix3[2] ||

			m_Matrix3[3] != comparableMatrix3.m_Matrix3[3] ||
			m_Matrix3[4] != comparableMatrix3.m_Matrix3[4] ||
			m_Matrix3[5] != comparableMatrix3.m_Matrix3[5] ||

			m_Matrix3[6] != comparableMatrix3.m_Matrix3[6] ||
			m_Matrix3[7] != comparableMatrix3.m_Matrix3[7] ||
			m_Matrix3[8] != comparableMatrix3.m_Matrix3[8]);
}



void Matrix3::GetBasisVectors(Vector2& firstRow, Vector2& secondRow, Vector2& thirdRow) const
{
	firstRow.X = m_Matrix3[0];
	firstRow.Y = m_Matrix3[1];

	secondRow.X = m_Matrix3[3];
	secondRow.Y = m_Matrix3[4];

	thirdRow.X = m_Matrix3[6];
	thirdRow.Y = m_Matrix3[7];
}



void Matrix3::SetBasisVectors(const Vector2& firstRow, const Vector2& secondRow, const Vector2& thirdRow)
{
	m_Matrix3[0] = firstRow.X;
	m_Matrix3[1] = firstRow.Y;

	m_Matrix3[3] = secondRow.X;
	m_Matrix3[4] = secondRow.Y;

	m_Matrix3[6] = thirdRow.X;
	m_Matrix3[7] = thirdRow.Y;
}



bool Matrix3::IsSymmetric() const
{
	Matrix3 transposeMatrix3;
	GetTranspose(transposeMatrix3);

	if ((*this) == transposeMatrix3)
	{
		return true;
	}

	return false;
}



bool Matrix3::IsSkewSymmetric() const
{
	Matrix3 transposeMatrix3;
	GetTranspose(transposeMatrix3);

	Matrix3 negatedTransposeMatrix3 = transposeMatrix3 * -1.0f;

	if ((*this) == negatedTransposeMatrix3)
	{
		return true;
	}

	return false;
}



bool Matrix3::IsOrthogonal() const
{
	Matrix3 inverseMatrix3;

	if (HasInverse(inverseMatrix3))
	{
		Matrix3 transposeMatrix3;
		GetTranspose(transposeMatrix3);

		if (inverseMatrix3 == transposeMatrix3)
		{
			return true;
		}
	}

	return false;
}