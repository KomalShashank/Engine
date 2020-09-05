#include "Engine/Math/MatrixMath/Matrix4.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"



Matrix4::Matrix4()
{

}



Matrix4::Matrix4(const float* matrix4)
{
	m_Matrix4[0] = matrix4[0];
	m_Matrix4[1] = matrix4[1];
	m_Matrix4[2] = matrix4[2];
	m_Matrix4[3] = matrix4[3];

	m_Matrix4[4] = matrix4[4];
	m_Matrix4[5] = matrix4[5];
	m_Matrix4[6] = matrix4[6];
	m_Matrix4[7] = matrix4[7];

	m_Matrix4[8] = matrix4[8];
	m_Matrix4[9] = matrix4[9];
	m_Matrix4[10] = matrix4[10];
	m_Matrix4[11] = matrix4[11];

	m_Matrix4[12] = matrix4[12];
	m_Matrix4[13] = matrix4[13];
	m_Matrix4[14] = matrix4[14];
	m_Matrix4[15] = matrix4[15];
}



Matrix4::Matrix4(const Matrix4& copyableMatrix4)
{
	m_Matrix4[0] = copyableMatrix4.m_Matrix4[0];
	m_Matrix4[1] = copyableMatrix4.m_Matrix4[1];
	m_Matrix4[2] = copyableMatrix4.m_Matrix4[2];
	m_Matrix4[3] = copyableMatrix4.m_Matrix4[3];

	m_Matrix4[4] = copyableMatrix4.m_Matrix4[4];
	m_Matrix4[5] = copyableMatrix4.m_Matrix4[5];
	m_Matrix4[6] = copyableMatrix4.m_Matrix4[6];
	m_Matrix4[7] = copyableMatrix4.m_Matrix4[7];

	m_Matrix4[8] = copyableMatrix4.m_Matrix4[8];
	m_Matrix4[9] = copyableMatrix4.m_Matrix4[9];
	m_Matrix4[10] = copyableMatrix4.m_Matrix4[10];
	m_Matrix4[11] = copyableMatrix4.m_Matrix4[11];

	m_Matrix4[12] = copyableMatrix4.m_Matrix4[12];
	m_Matrix4[13] = copyableMatrix4.m_Matrix4[13];
	m_Matrix4[14] = copyableMatrix4.m_Matrix4[14];
	m_Matrix4[15] = copyableMatrix4.m_Matrix4[15];
}



Matrix4 Matrix4::IdentityMatrix4()
{
	Matrix4 identity;

	identity.m_Matrix4[0] = 1.0f;
	identity.m_Matrix4[1] = 0.0f;
	identity.m_Matrix4[2] = 0.0f;
	identity.m_Matrix4[3] = 0.0f;

	identity.m_Matrix4[4] = 0.0f;
	identity.m_Matrix4[5] = 1.0f;
	identity.m_Matrix4[6] = 0.0f;
	identity.m_Matrix4[7] = 0.0f;

	identity.m_Matrix4[8] = 0.0f;
	identity.m_Matrix4[9] = 0.0f;
	identity.m_Matrix4[10] = 1.0f;
	identity.m_Matrix4[11] = 0.0f;

	identity.m_Matrix4[12] = 0.0f;
	identity.m_Matrix4[13] = 0.0f;
	identity.m_Matrix4[14] = 0.0f;
	identity.m_Matrix4[15] = 1.0f;

	return identity;
}



float Matrix4::GetDeterminant() const
{
	float determinant = 0.0f;

	const float& A_11 = m_Matrix4[0];
	const float& A_12 = m_Matrix4[1];
	const float& A_13 = m_Matrix4[2];
	const float& A_14 = m_Matrix4[3];

	const float& A_21 = m_Matrix4[4];
	const float& A_22 = m_Matrix4[5];
	const float& A_23 = m_Matrix4[6];
	const float& A_24 = m_Matrix4[7];

	const float& A_31 = m_Matrix4[8];
	const float& A_32 = m_Matrix4[9];
	const float& A_33 = m_Matrix4[10];
	const float& A_34 = m_Matrix4[11];

	const float& A_41 = m_Matrix4[12];
	const float& A_42 = m_Matrix4[13];
	const float& A_43 = m_Matrix4[14];
	const float& A_44 = m_Matrix4[15];

	determinant += A_11 * A_22 * A_33 * A_44;
	determinant += A_11 * A_23 * A_34 * A_42;
	determinant += A_11 * A_24 * A_32 * A_43;

	determinant += A_12 * A_21 * A_34 * A_43;
	determinant += A_12 * A_23 * A_31 * A_44;
	determinant += A_12 * A_24 * A_33 * A_41;

	determinant += A_13 * A_21 * A_32 * A_44;
	determinant += A_13 * A_22 * A_34 * A_41;
	determinant += A_13 * A_24 * A_31 * A_42;

	determinant += A_14 * A_21 * A_33 * A_42;
	determinant += A_14 * A_22 * A_31 * A_43;
	determinant += A_14 * A_23 * A_32 * A_41;

	determinant -= A_11 * A_22 * A_34 * A_43;
	determinant -= A_11 * A_23 * A_32 * A_44;
	determinant -= A_11 * A_24 * A_33 * A_42;

	determinant -= A_12 * A_21 * A_33 * A_44;
	determinant -= A_12 * A_23 * A_34 * A_41;
	determinant -= A_12 * A_24 * A_31 * A_43;
	
	determinant -= A_13 * A_21 * A_34 * A_42;
	determinant -= A_13 * A_22 * A_31 * A_44;
	determinant -= A_13 * A_24 * A_32 * A_41;

	determinant -= A_14 * A_21 * A_32 * A_43;
	determinant -= A_14 * A_22 * A_33 * A_41;
	determinant -= A_14 * A_23 * A_31 * A_42;

	return determinant;
}



float Matrix4::GetTrace() const
{
	return (m_Matrix4[0] + m_Matrix4[5] + m_Matrix4[10] + m_Matrix4[15]);
}



bool Matrix4::HasInverse(Matrix4& outputInverseMatrix4) const
{
	float determinant = GetDeterminant();

	if (determinant == 0.0f)
	{
		return false;
	}

	const float& A_11 = m_Matrix4[0];
	const float& A_12 = m_Matrix4[1];
	const float& A_13 = m_Matrix4[2];
	const float& A_14 = m_Matrix4[3];

	const float& A_21 = m_Matrix4[4];
	const float& A_22 = m_Matrix4[5];
	const float& A_23 = m_Matrix4[6];
	const float& A_24 = m_Matrix4[7];

	const float& A_31 = m_Matrix4[8];
	const float& A_32 = m_Matrix4[9];
	const float& A_33 = m_Matrix4[10];
	const float& A_34 = m_Matrix4[11];

	const float& A_41 = m_Matrix4[12];
	const float& A_42 = m_Matrix4[13];
	const float& A_43 = m_Matrix4[14];
	const float& A_44 = m_Matrix4[15];

	float oneOverDeterminant = 1.0f / determinant;

	outputInverseMatrix4.m_Matrix4[0] = ((A_22 * A_33 * A_44) + (A_23 * A_34 * A_42) + (A_24 * A_32 * A_43) - (A_22 * A_34 * A_43) - (A_23 * A_32 * A_44) - (A_24 * A_33 * A_42)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[1] = ((A_12 * A_34 * A_43) + (A_13 * A_32 * A_44) + (A_14 * A_33 * A_42) - (A_12 * A_33 * A_44) - (A_13 * A_34 * A_42) - (A_14 * A_32 * A_43)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[2] = ((A_12 * A_23 * A_44) + (A_13 * A_24 * A_42) + (A_14 * A_22 * A_43) - (A_12 * A_24 * A_43) - (A_13 * A_22 * A_44) - (A_14 * A_23 * A_42)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[3] = ((A_12 * A_24 * A_33) + (A_13 * A_22 * A_34) + (A_14 * A_23 * A_32) - (A_12 * A_23 * A_34) - (A_13 * A_24 * A_32) - (A_14 * A_22 * A_33)) * oneOverDeterminant;

	outputInverseMatrix4.m_Matrix4[4] = ((A_21 * A_34 * A_43) + (A_23 * A_31 * A_44) + (A_24 * A_33 * A_41) - (A_21 * A_33 * A_44) - (A_23 * A_34 * A_41) - (A_24 * A_31 * A_43)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[5] = ((A_11 * A_33 * A_44) + (A_13 * A_34 * A_41) + (A_14 * A_31 * A_43) - (A_11 * A_34 * A_43) - (A_13 * A_31 * A_44) - (A_14 * A_33 * A_41)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[6] = ((A_11 * A_24 * A_43) + (A_13 * A_21 * A_44) + (A_14 * A_23 * A_41) - (A_11 * A_23 * A_44) - (A_13 * A_24 * A_41) - (A_14 * A_21 * A_43)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[7] = ((A_11 * A_23 * A_34) + (A_13 * A_24 * A_31) + (A_14 * A_21 * A_33) - (A_11 * A_24 * A_33) - (A_13 * A_21 * A_34) - (A_14 * A_23 * A_31)) * oneOverDeterminant;

	outputInverseMatrix4.m_Matrix4[8] = ((A_21 * A_32 * A_44) + (A_22 * A_34 * A_41) + (A_24 * A_31 * A_42) - (A_21 * A_34 * A_42) - (A_22 * A_31 * A_44) - (A_24 * A_32 * A_41)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[9] = ((A_11 * A_34 * A_42) + (A_12 * A_31 * A_44) + (A_14 * A_32 * A_41) - (A_11 * A_32 * A_44) - (A_12 * A_34 * A_41) - (A_14 * A_31 * A_42)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[10] = ((A_11 * A_22 * A_44) + (A_12 * A_24 * A_41) + (A_14 * A_21 * A_42) - (A_11 * A_24 * A_42) - (A_12 * A_21 * A_44) - (A_14 * A_22 * A_41)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[11] = ((A_11 * A_24 * A_32) + (A_12 * A_21 * A_34) + (A_14 * A_22 * A_31) - (A_11 * A_22 * A_34) - (A_12 * A_24 * A_31) - (A_14 * A_21 * A_32)) * oneOverDeterminant;

	outputInverseMatrix4.m_Matrix4[12] = ((A_21 * A_33 * A_42) + (A_22 * A_31 * A_43) + (A_23 * A_32 * A_41) - (A_21 * A_32 * A_43) - (A_22 * A_33 * A_41) - (A_23 * A_31 * A_42)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[13] = ((A_11 * A_32 * A_43) + (A_12 * A_33 * A_41) + (A_13 * A_31 * A_42) - (A_11 * A_33 * A_42) - (A_12 * A_31 * A_43) - (A_13 * A_32 * A_41)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[14] = ((A_11 * A_23 * A_42) + (A_12 * A_21 * A_43) + (A_13 * A_22 * A_41) - (A_11 * A_22 * A_43) - (A_12 * A_23 * A_41) - (A_13 * A_21 * A_42)) * oneOverDeterminant;
	outputInverseMatrix4.m_Matrix4[15] = ((A_11 * A_22 * A_33) + (A_12 * A_23 * A_31) + (A_13 * A_21 * A_32) - (A_11 * A_23 * A_32) - (A_12 * A_21 * A_33) - (A_13 * A_22 * A_31)) * oneOverDeterminant;

	return true;
}



Matrix4 Matrix4::GetTranspose() const
{
	Matrix4 outputTransposeMatrix4 = Matrix4::IdentityMatrix4();
	
	outputTransposeMatrix4.m_Matrix4[0] = m_Matrix4[0];
	outputTransposeMatrix4.m_Matrix4[1] = m_Matrix4[4];
	outputTransposeMatrix4.m_Matrix4[2] = m_Matrix4[8];
	outputTransposeMatrix4.m_Matrix4[3] = m_Matrix4[12];

	outputTransposeMatrix4.m_Matrix4[4] = m_Matrix4[1];
	outputTransposeMatrix4.m_Matrix4[5] = m_Matrix4[5];
	outputTransposeMatrix4.m_Matrix4[6] = m_Matrix4[9];
	outputTransposeMatrix4.m_Matrix4[7] = m_Matrix4[13];

	outputTransposeMatrix4.m_Matrix4[8] = m_Matrix4[2];
	outputTransposeMatrix4.m_Matrix4[9] = m_Matrix4[6];
	outputTransposeMatrix4.m_Matrix4[10] = m_Matrix4[10];
	outputTransposeMatrix4.m_Matrix4[11] = m_Matrix4[14];

	outputTransposeMatrix4.m_Matrix4[12] = m_Matrix4[3];
	outputTransposeMatrix4.m_Matrix4[13] = m_Matrix4[7];
	outputTransposeMatrix4.m_Matrix4[14] = m_Matrix4[11];
	outputTransposeMatrix4.m_Matrix4[15] = m_Matrix4[15];

	return outputTransposeMatrix4;
}



Matrix4 Matrix4::Multiply(const Matrix4& firstMatrix4, const Matrix4& secondMatrix4)
{
	Matrix4 resultMatrix4 = Matrix4::IdentityMatrix4();
	
	resultMatrix4.m_Matrix4[0] = (firstMatrix4.m_Matrix4[0] * secondMatrix4.m_Matrix4[0]) + (firstMatrix4.m_Matrix4[1] * secondMatrix4.m_Matrix4[4]) + (firstMatrix4.m_Matrix4[2] * secondMatrix4.m_Matrix4[8]) + (firstMatrix4.m_Matrix4[3] * secondMatrix4.m_Matrix4[12]);
	resultMatrix4.m_Matrix4[1] = (firstMatrix4.m_Matrix4[0] * secondMatrix4.m_Matrix4[1]) + (firstMatrix4.m_Matrix4[1] * secondMatrix4.m_Matrix4[5]) + (firstMatrix4.m_Matrix4[2] * secondMatrix4.m_Matrix4[9]) + (firstMatrix4.m_Matrix4[3] * secondMatrix4.m_Matrix4[13]);
	resultMatrix4.m_Matrix4[2] = (firstMatrix4.m_Matrix4[0] * secondMatrix4.m_Matrix4[2]) + (firstMatrix4.m_Matrix4[1] * secondMatrix4.m_Matrix4[6]) + (firstMatrix4.m_Matrix4[2] * secondMatrix4.m_Matrix4[10]) + (firstMatrix4.m_Matrix4[3] * secondMatrix4.m_Matrix4[14]);
	resultMatrix4.m_Matrix4[3] = (firstMatrix4.m_Matrix4[0] * secondMatrix4.m_Matrix4[3]) + (firstMatrix4.m_Matrix4[1] * secondMatrix4.m_Matrix4[7]) + (firstMatrix4.m_Matrix4[2] * secondMatrix4.m_Matrix4[11]) + (firstMatrix4.m_Matrix4[3] * secondMatrix4.m_Matrix4[15]);

	resultMatrix4.m_Matrix4[4] = (firstMatrix4.m_Matrix4[4] * secondMatrix4.m_Matrix4[0]) + (firstMatrix4.m_Matrix4[5] * secondMatrix4.m_Matrix4[4]) + (firstMatrix4.m_Matrix4[6] * secondMatrix4.m_Matrix4[8]) + (firstMatrix4.m_Matrix4[7] * secondMatrix4.m_Matrix4[12]);
	resultMatrix4.m_Matrix4[5] = (firstMatrix4.m_Matrix4[4] * secondMatrix4.m_Matrix4[1]) + (firstMatrix4.m_Matrix4[5] * secondMatrix4.m_Matrix4[5]) + (firstMatrix4.m_Matrix4[6] * secondMatrix4.m_Matrix4[9]) + (firstMatrix4.m_Matrix4[7] * secondMatrix4.m_Matrix4[13]);
	resultMatrix4.m_Matrix4[6] = (firstMatrix4.m_Matrix4[4] * secondMatrix4.m_Matrix4[2]) + (firstMatrix4.m_Matrix4[5] * secondMatrix4.m_Matrix4[6]) + (firstMatrix4.m_Matrix4[6] * secondMatrix4.m_Matrix4[10]) + (firstMatrix4.m_Matrix4[7] * secondMatrix4.m_Matrix4[14]);
	resultMatrix4.m_Matrix4[7] = (firstMatrix4.m_Matrix4[4] * secondMatrix4.m_Matrix4[3]) + (firstMatrix4.m_Matrix4[5] * secondMatrix4.m_Matrix4[7]) + (firstMatrix4.m_Matrix4[6] * secondMatrix4.m_Matrix4[11]) + (firstMatrix4.m_Matrix4[7] * secondMatrix4.m_Matrix4[15]);

	resultMatrix4.m_Matrix4[8] = (firstMatrix4.m_Matrix4[8] * secondMatrix4.m_Matrix4[0]) + (firstMatrix4.m_Matrix4[9] * secondMatrix4.m_Matrix4[4]) + (firstMatrix4.m_Matrix4[10] * secondMatrix4.m_Matrix4[8]) + (firstMatrix4.m_Matrix4[11] * secondMatrix4.m_Matrix4[12]);
	resultMatrix4.m_Matrix4[9] = (firstMatrix4.m_Matrix4[8] * secondMatrix4.m_Matrix4[1]) + (firstMatrix4.m_Matrix4[9] * secondMatrix4.m_Matrix4[5]) + (firstMatrix4.m_Matrix4[10] * secondMatrix4.m_Matrix4[9]) + (firstMatrix4.m_Matrix4[11] * secondMatrix4.m_Matrix4[13]);
	resultMatrix4.m_Matrix4[10] = (firstMatrix4.m_Matrix4[8] * secondMatrix4.m_Matrix4[2]) + (firstMatrix4.m_Matrix4[9] * secondMatrix4.m_Matrix4[6]) + (firstMatrix4.m_Matrix4[10] * secondMatrix4.m_Matrix4[10]) + (firstMatrix4.m_Matrix4[11] * secondMatrix4.m_Matrix4[14]);
	resultMatrix4.m_Matrix4[11] = (firstMatrix4.m_Matrix4[8] * secondMatrix4.m_Matrix4[3]) + (firstMatrix4.m_Matrix4[9] * secondMatrix4.m_Matrix4[7]) + (firstMatrix4.m_Matrix4[10] * secondMatrix4.m_Matrix4[11]) + (firstMatrix4.m_Matrix4[11] * secondMatrix4.m_Matrix4[15]);

	resultMatrix4.m_Matrix4[12] = (firstMatrix4.m_Matrix4[12] * secondMatrix4.m_Matrix4[0]) + (firstMatrix4.m_Matrix4[13] * secondMatrix4.m_Matrix4[4]) + (firstMatrix4.m_Matrix4[14] * secondMatrix4.m_Matrix4[8]) + (firstMatrix4.m_Matrix4[15] * secondMatrix4.m_Matrix4[12]);
	resultMatrix4.m_Matrix4[13] = (firstMatrix4.m_Matrix4[12] * secondMatrix4.m_Matrix4[1]) + (firstMatrix4.m_Matrix4[13] * secondMatrix4.m_Matrix4[5]) + (firstMatrix4.m_Matrix4[14] * secondMatrix4.m_Matrix4[9]) + (firstMatrix4.m_Matrix4[15] * secondMatrix4.m_Matrix4[13]);
	resultMatrix4.m_Matrix4[14] = (firstMatrix4.m_Matrix4[12] * secondMatrix4.m_Matrix4[2]) + (firstMatrix4.m_Matrix4[13] * secondMatrix4.m_Matrix4[6]) + (firstMatrix4.m_Matrix4[14] * secondMatrix4.m_Matrix4[10]) + (firstMatrix4.m_Matrix4[15] * secondMatrix4.m_Matrix4[14]);
	resultMatrix4.m_Matrix4[15] = (firstMatrix4.m_Matrix4[12] * secondMatrix4.m_Matrix4[3]) + (firstMatrix4.m_Matrix4[13] * secondMatrix4.m_Matrix4[7]) + (firstMatrix4.m_Matrix4[14] * secondMatrix4.m_Matrix4[11]) + (firstMatrix4.m_Matrix4[15] * secondMatrix4.m_Matrix4[15]);

	return resultMatrix4;
}



Vector3 Matrix4::TransformVector3(const Vector3& transformableVector3, float tComponent) const
{
	Vector4 transformableVector4 = Vector4(transformableVector3.X, transformableVector3.Y, transformableVector3.Z, tComponent);

	Vector4 transformedVector4;
	transformedVector4.X = transformableVector4.X * m_Matrix4[0] + transformableVector4.Y * m_Matrix4[4] + transformableVector4.Z * m_Matrix4[8] + transformableVector4.T * m_Matrix4[12];
	transformedVector4.Y = transformableVector4.X * m_Matrix4[1] + transformableVector4.Y * m_Matrix4[5] + transformableVector4.Z * m_Matrix4[9] + transformableVector4.T * m_Matrix4[13];
	transformedVector4.Z = transformableVector4.X * m_Matrix4[2] + transformableVector4.Y * m_Matrix4[6] + transformableVector4.Z * m_Matrix4[10] + transformableVector4.T * m_Matrix4[14];
	transformedVector4.T = transformableVector4.X * m_Matrix4[3] + transformableVector4.Y * m_Matrix4[7] + transformableVector4.Z * m_Matrix4[11] + transformableVector4.T * m_Matrix4[15];

	return Vector3(transformedVector4.X, transformedVector4.Y, transformedVector4.Z);
}



Matrix4 Matrix4::SetTranslationMatrix4(const Vector3& translationPosition)
{
	Matrix4 resultMatrix4 = Matrix4::IdentityMatrix4();

	resultMatrix4.m_Matrix4[12] = translationPosition.X;
	resultMatrix4.m_Matrix4[13] = translationPosition.Y;
	resultMatrix4.m_Matrix4[14] = translationPosition.Z;

	return resultMatrix4;
}



Matrix4 Matrix4::InterpolateMatrices4(const Matrix4& firstMatrix4, const Matrix4& secondMatrix4, float interpolationFactor)
{
	Vector3 firstMatrix4firstRow;
	Vector3 firstMatrix4secondRow;
	Vector3 firstMatrix4thirdRow;
	Vector3 firstMatrix4fourthRow;
	firstMatrix4.GetBasisVectors(firstMatrix4firstRow, firstMatrix4secondRow, firstMatrix4thirdRow, firstMatrix4fourthRow);

	Vector3 secondMatrix4firstRow;
	Vector3 secondMatrix4secondRow;
	Vector3 secondMatrix4thirdRow;
	Vector3 secondMatrix4fourthRow;
	secondMatrix4.GetBasisVectors(secondMatrix4firstRow, secondMatrix4secondRow, secondMatrix4thirdRow, secondMatrix4fourthRow);

	Vector3 firstRow = SphericallyInterpolateIn3D(firstMatrix4firstRow, secondMatrix4firstRow, interpolationFactor);
	Vector3 secondRow = SphericallyInterpolateIn3D(firstMatrix4secondRow, secondMatrix4secondRow, interpolationFactor);
	Vector3 thirdRow = SphericallyInterpolateIn3D(firstMatrix4thirdRow, secondMatrix4thirdRow, interpolationFactor);
	Vector3 fourthRow = LinearlyInterpolateIn3D(firstMatrix4fourthRow, secondMatrix4fourthRow, interpolationFactor);

	Matrix4 resultantMatrix4 = Matrix4::IdentityMatrix4();
	resultantMatrix4.SetBasisVectors(firstRow, secondRow, thirdRow, fourthRow);

	return resultantMatrix4;
}



Matrix4 Matrix4::operator+(const Matrix4& addableMatrix4) const
{
	Matrix4 resultantMatrix4;

	resultantMatrix4.m_Matrix4[0] = m_Matrix4[0] + addableMatrix4.m_Matrix4[0];
	resultantMatrix4.m_Matrix4[1] = m_Matrix4[1] + addableMatrix4.m_Matrix4[1];
	resultantMatrix4.m_Matrix4[2] = m_Matrix4[2] + addableMatrix4.m_Matrix4[2];
	resultantMatrix4.m_Matrix4[3] = m_Matrix4[3] + addableMatrix4.m_Matrix4[3];

	resultantMatrix4.m_Matrix4[4] = m_Matrix4[4] + addableMatrix4.m_Matrix4[4];
	resultantMatrix4.m_Matrix4[5] = m_Matrix4[5] + addableMatrix4.m_Matrix4[5];
	resultantMatrix4.m_Matrix4[6] = m_Matrix4[6] + addableMatrix4.m_Matrix4[6];
	resultantMatrix4.m_Matrix4[7] = m_Matrix4[7] + addableMatrix4.m_Matrix4[7];

	resultantMatrix4.m_Matrix4[8] = m_Matrix4[8] + addableMatrix4.m_Matrix4[8];
	resultantMatrix4.m_Matrix4[9] = m_Matrix4[9] + addableMatrix4.m_Matrix4[9];
	resultantMatrix4.m_Matrix4[10] = m_Matrix4[10] + addableMatrix4.m_Matrix4[10];
	resultantMatrix4.m_Matrix4[11] = m_Matrix4[11] + addableMatrix4.m_Matrix4[11];

	resultantMatrix4.m_Matrix4[12] = m_Matrix4[12] + addableMatrix4.m_Matrix4[12];
	resultantMatrix4.m_Matrix4[13] = m_Matrix4[13] + addableMatrix4.m_Matrix4[13];
	resultantMatrix4.m_Matrix4[14] = m_Matrix4[14] + addableMatrix4.m_Matrix4[14];
	resultantMatrix4.m_Matrix4[15] = m_Matrix4[15] + addableMatrix4.m_Matrix4[15];

	return resultantMatrix4;
}



Matrix4 Matrix4::operator-(const Matrix4& subtractableMatrix4) const
{
	Matrix4 resultantMatrix4;

	resultantMatrix4.m_Matrix4[0] = m_Matrix4[0] - subtractableMatrix4.m_Matrix4[0];
	resultantMatrix4.m_Matrix4[1] = m_Matrix4[1] - subtractableMatrix4.m_Matrix4[1];
	resultantMatrix4.m_Matrix4[2] = m_Matrix4[2] - subtractableMatrix4.m_Matrix4[2];
	resultantMatrix4.m_Matrix4[3] = m_Matrix4[3] - subtractableMatrix4.m_Matrix4[3];

	resultantMatrix4.m_Matrix4[4] = m_Matrix4[4] - subtractableMatrix4.m_Matrix4[4];
	resultantMatrix4.m_Matrix4[5] = m_Matrix4[5] - subtractableMatrix4.m_Matrix4[5];
	resultantMatrix4.m_Matrix4[6] = m_Matrix4[6] - subtractableMatrix4.m_Matrix4[6];
	resultantMatrix4.m_Matrix4[7] = m_Matrix4[7] - subtractableMatrix4.m_Matrix4[7];

	resultantMatrix4.m_Matrix4[8] = m_Matrix4[8] - subtractableMatrix4.m_Matrix4[8];
	resultantMatrix4.m_Matrix4[9] = m_Matrix4[9] - subtractableMatrix4.m_Matrix4[9];
	resultantMatrix4.m_Matrix4[10] = m_Matrix4[10] - subtractableMatrix4.m_Matrix4[10];
	resultantMatrix4.m_Matrix4[11] = m_Matrix4[11] - subtractableMatrix4.m_Matrix4[11];

	resultantMatrix4.m_Matrix4[12] = m_Matrix4[12] - subtractableMatrix4.m_Matrix4[12];
	resultantMatrix4.m_Matrix4[13] = m_Matrix4[13] - subtractableMatrix4.m_Matrix4[13];
	resultantMatrix4.m_Matrix4[14] = m_Matrix4[14] - subtractableMatrix4.m_Matrix4[14];
	resultantMatrix4.m_Matrix4[15] = m_Matrix4[15] - subtractableMatrix4.m_Matrix4[15];

	return resultantMatrix4;
}



Matrix4 Matrix4::operator*(float multipliableValue) const
{
	Matrix4 resultantMatrix4;

	resultantMatrix4.m_Matrix4[0] = m_Matrix4[0] * multipliableValue;
	resultantMatrix4.m_Matrix4[1] = m_Matrix4[1] * multipliableValue;
	resultantMatrix4.m_Matrix4[2] = m_Matrix4[2] * multipliableValue;
	resultantMatrix4.m_Matrix4[3] = m_Matrix4[3] * multipliableValue;

	resultantMatrix4.m_Matrix4[4] = m_Matrix4[4] * multipliableValue;
	resultantMatrix4.m_Matrix4[5] = m_Matrix4[5] * multipliableValue;
	resultantMatrix4.m_Matrix4[6] = m_Matrix4[6] * multipliableValue;
	resultantMatrix4.m_Matrix4[7] = m_Matrix4[7] * multipliableValue;

	resultantMatrix4.m_Matrix4[8] = m_Matrix4[8] * multipliableValue;
	resultantMatrix4.m_Matrix4[9] = m_Matrix4[9] * multipliableValue;
	resultantMatrix4.m_Matrix4[10] = m_Matrix4[10] * multipliableValue;
	resultantMatrix4.m_Matrix4[11] = m_Matrix4[11] * multipliableValue;

	resultantMatrix4.m_Matrix4[12] = m_Matrix4[12] * multipliableValue;
	resultantMatrix4.m_Matrix4[13] = m_Matrix4[13] * multipliableValue;
	resultantMatrix4.m_Matrix4[14] = m_Matrix4[14] * multipliableValue;
	resultantMatrix4.m_Matrix4[15] = m_Matrix4[15] * multipliableValue;

	return resultantMatrix4;
}



Matrix4 Matrix4::operator/(float dividableValue) const
{
	Matrix4 resultantMatrix4;

	resultantMatrix4.m_Matrix4[0] = m_Matrix4[0] / dividableValue;
	resultantMatrix4.m_Matrix4[1] = m_Matrix4[1] / dividableValue;
	resultantMatrix4.m_Matrix4[2] = m_Matrix4[2] / dividableValue;
	resultantMatrix4.m_Matrix4[3] = m_Matrix4[3] / dividableValue;

	resultantMatrix4.m_Matrix4[4] = m_Matrix4[4] / dividableValue;
	resultantMatrix4.m_Matrix4[5] = m_Matrix4[5] / dividableValue;
	resultantMatrix4.m_Matrix4[6] = m_Matrix4[6] / dividableValue;
	resultantMatrix4.m_Matrix4[7] = m_Matrix4[7] / dividableValue;

	resultantMatrix4.m_Matrix4[8] = m_Matrix4[8] / dividableValue;
	resultantMatrix4.m_Matrix4[9] = m_Matrix4[9] / dividableValue;
	resultantMatrix4.m_Matrix4[10] = m_Matrix4[10] / dividableValue;
	resultantMatrix4.m_Matrix4[11] = m_Matrix4[11] / dividableValue;

	resultantMatrix4.m_Matrix4[12] = m_Matrix4[12] / dividableValue;
	resultantMatrix4.m_Matrix4[13] = m_Matrix4[13] / dividableValue;
	resultantMatrix4.m_Matrix4[14] = m_Matrix4[14] / dividableValue;
	resultantMatrix4.m_Matrix4[15] = m_Matrix4[15] / dividableValue;

	return resultantMatrix4;
}



void Matrix4::operator+=(const Matrix4& addableMatrix4)
{
	m_Matrix4[0] += addableMatrix4.m_Matrix4[0];
	m_Matrix4[1] += addableMatrix4.m_Matrix4[1];
	m_Matrix4[2] += addableMatrix4.m_Matrix4[2];
	m_Matrix4[3] += addableMatrix4.m_Matrix4[3];

	m_Matrix4[4] += addableMatrix4.m_Matrix4[4];
	m_Matrix4[5] += addableMatrix4.m_Matrix4[5];
	m_Matrix4[6] += addableMatrix4.m_Matrix4[6];
	m_Matrix4[7] += addableMatrix4.m_Matrix4[7];

	m_Matrix4[8] += addableMatrix4.m_Matrix4[8];
	m_Matrix4[9] += addableMatrix4.m_Matrix4[9];
	m_Matrix4[10] += addableMatrix4.m_Matrix4[10];
	m_Matrix4[11] += addableMatrix4.m_Matrix4[11];

	m_Matrix4[12] += addableMatrix4.m_Matrix4[12];
	m_Matrix4[13] += addableMatrix4.m_Matrix4[13];
	m_Matrix4[14] += addableMatrix4.m_Matrix4[14];
	m_Matrix4[15] += addableMatrix4.m_Matrix4[15];
}



void Matrix4::operator-=(const Matrix4& subtractableMatrix4)
{
	m_Matrix4[0] -= subtractableMatrix4.m_Matrix4[0];
	m_Matrix4[1] -= subtractableMatrix4.m_Matrix4[1];
	m_Matrix4[2] -= subtractableMatrix4.m_Matrix4[2];
	m_Matrix4[3] -= subtractableMatrix4.m_Matrix4[3];

	m_Matrix4[4] -= subtractableMatrix4.m_Matrix4[4];
	m_Matrix4[5] -= subtractableMatrix4.m_Matrix4[5];
	m_Matrix4[6] -= subtractableMatrix4.m_Matrix4[6];
	m_Matrix4[7] -= subtractableMatrix4.m_Matrix4[7];

	m_Matrix4[8] -= subtractableMatrix4.m_Matrix4[8];
	m_Matrix4[9] -= subtractableMatrix4.m_Matrix4[9];
	m_Matrix4[10] -= subtractableMatrix4.m_Matrix4[10];
	m_Matrix4[11] -= subtractableMatrix4.m_Matrix4[11];

	m_Matrix4[12] -= subtractableMatrix4.m_Matrix4[12];
	m_Matrix4[13] -= subtractableMatrix4.m_Matrix4[13];
	m_Matrix4[14] -= subtractableMatrix4.m_Matrix4[14];
	m_Matrix4[15] -= subtractableMatrix4.m_Matrix4[15];
}



void Matrix4::operator*=(float multipliableValue)
{
	m_Matrix4[0] *= multipliableValue;
	m_Matrix4[1] *= multipliableValue;
	m_Matrix4[2] *= multipliableValue;
	m_Matrix4[3] *= multipliableValue;

	m_Matrix4[4] *= multipliableValue;
	m_Matrix4[5] *= multipliableValue;
	m_Matrix4[6] *= multipliableValue;
	m_Matrix4[7] *= multipliableValue;

	m_Matrix4[8] *= multipliableValue;
	m_Matrix4[9] *= multipliableValue;
	m_Matrix4[10] *= multipliableValue;
	m_Matrix4[11] *= multipliableValue;

	m_Matrix4[12] *= multipliableValue;
	m_Matrix4[13] *= multipliableValue;
	m_Matrix4[14] *= multipliableValue;
	m_Matrix4[15] *= multipliableValue;
}



void Matrix4::operator/=(float dividableValue)
{
	m_Matrix4[0] /= dividableValue;
	m_Matrix4[1] /= dividableValue;
	m_Matrix4[2] /= dividableValue;
	m_Matrix4[3] /= dividableValue;

	m_Matrix4[4] /= dividableValue;
	m_Matrix4[5] /= dividableValue;
	m_Matrix4[6] /= dividableValue;
	m_Matrix4[7] /= dividableValue;

	m_Matrix4[8] /= dividableValue;
	m_Matrix4[9] /= dividableValue;
	m_Matrix4[10] /= dividableValue;
	m_Matrix4[11] /= dividableValue;

	m_Matrix4[12] /= dividableValue;
	m_Matrix4[13] /= dividableValue;
	m_Matrix4[14] /= dividableValue;
	m_Matrix4[15] /= dividableValue;
}



bool Matrix4::operator==(const Matrix4& comparableMatrix4) const
{
	return (m_Matrix4[0] == comparableMatrix4.m_Matrix4[0] &&
			m_Matrix4[1] == comparableMatrix4.m_Matrix4[1] &&
			m_Matrix4[2] == comparableMatrix4.m_Matrix4[2] &&
			m_Matrix4[3] == comparableMatrix4.m_Matrix4[3] &&

			m_Matrix4[4] == comparableMatrix4.m_Matrix4[4] &&
			m_Matrix4[5] == comparableMatrix4.m_Matrix4[5] &&
			m_Matrix4[6] == comparableMatrix4.m_Matrix4[6] &&
			m_Matrix4[7] == comparableMatrix4.m_Matrix4[7] &&

			m_Matrix4[8] == comparableMatrix4.m_Matrix4[8] &&
			m_Matrix4[9] == comparableMatrix4.m_Matrix4[9] &&
			m_Matrix4[10] == comparableMatrix4.m_Matrix4[10] &&
			m_Matrix4[11] == comparableMatrix4.m_Matrix4[11] &&

			m_Matrix4[12] == comparableMatrix4.m_Matrix4[12] &&
			m_Matrix4[13] == comparableMatrix4.m_Matrix4[13] &&
			m_Matrix4[14] == comparableMatrix4.m_Matrix4[14] &&
			m_Matrix4[15] == comparableMatrix4.m_Matrix4[15]);
}



bool Matrix4::operator!=(const Matrix4& comparableMatrix4) const
{
	return (m_Matrix4[0] != comparableMatrix4.m_Matrix4[0] ||
			m_Matrix4[1] != comparableMatrix4.m_Matrix4[1] ||
			m_Matrix4[2] != comparableMatrix4.m_Matrix4[2] ||
			m_Matrix4[3] != comparableMatrix4.m_Matrix4[3] ||

			m_Matrix4[4] != comparableMatrix4.m_Matrix4[4] ||
			m_Matrix4[5] != comparableMatrix4.m_Matrix4[5] ||
			m_Matrix4[6] != comparableMatrix4.m_Matrix4[6] ||
			m_Matrix4[7] != comparableMatrix4.m_Matrix4[7] ||

			m_Matrix4[8] != comparableMatrix4.m_Matrix4[8] ||
			m_Matrix4[9] != comparableMatrix4.m_Matrix4[9] ||
			m_Matrix4[10] != comparableMatrix4.m_Matrix4[10] ||
			m_Matrix4[11] != comparableMatrix4.m_Matrix4[11] ||

			m_Matrix4[12] != comparableMatrix4.m_Matrix4[12] ||
			m_Matrix4[13] != comparableMatrix4.m_Matrix4[13] ||
			m_Matrix4[14] != comparableMatrix4.m_Matrix4[14] ||
			m_Matrix4[15] != comparableMatrix4.m_Matrix4[15]);
}



Matrix4 Matrix4::operator*(const Matrix4& multipliableMatrix4) const
{
	Matrix4 resultantMatrix4 = Matrix4::Multiply(*this, multipliableMatrix4);

	return resultantMatrix4;
}



void Matrix4::GetBasisVectors(Vector3& firstRow, Vector3& secondRow, Vector3& thirdRow, Vector3& fourthRow) const
{
	firstRow.X = m_Matrix4[0];
	firstRow.Y = m_Matrix4[1];
	firstRow.Z = m_Matrix4[2];

	secondRow.X = m_Matrix4[4];
	secondRow.Y = m_Matrix4[5];
	secondRow.Z = m_Matrix4[6];

	thirdRow.X = m_Matrix4[8];
	thirdRow.Y = m_Matrix4[9];
	thirdRow.Z = m_Matrix4[10];

	fourthRow.X = m_Matrix4[12];
	fourthRow.Y = m_Matrix4[13];
	fourthRow.Z = m_Matrix4[14];
}



void Matrix4::SetBasisVectors(const Vector3& firstRow, const Vector3& secondRow, const Vector3& thirdRow, const Vector3& fourthRow)
{
	m_Matrix4[0] = firstRow.X;
	m_Matrix4[1] = firstRow.Y;
	m_Matrix4[2] = firstRow.Z;

	m_Matrix4[4] = secondRow.X;
	m_Matrix4[5] = secondRow.Y;
	m_Matrix4[6] = secondRow.Z;

	m_Matrix4[8] = thirdRow.X;
	m_Matrix4[9] = thirdRow.Y;
	m_Matrix4[10] = thirdRow.Z;

	m_Matrix4[12] = fourthRow.X;
	m_Matrix4[13] = fourthRow.Y;
	m_Matrix4[14] = fourthRow.Z;
}



bool Matrix4::IsSymmetric() const
{
	Matrix4 transposeMatrix4;
	GetTranspose();

	if ((*this) == transposeMatrix4)
	{
		return true;
	}

	return false;
}



bool Matrix4::IsSkewSymmetric() const
{
	Matrix4 transposeMatrix4;
	GetTranspose();

	Matrix4 negatedTransposeMatrix4 = transposeMatrix4 * -1.0f;

	if ((*this) == negatedTransposeMatrix4)
	{
		return true;
	}

	return false;
}



bool Matrix4::IsOrthogonal() const
{
	Matrix4 inverseMatrix4;

	if (HasInverse(inverseMatrix4))
	{
		Matrix4 transposeMatrix4;
		GetTranspose();

		if (inverseMatrix4 == transposeMatrix4)
		{
			return true;
		}
	}

	return false;
}