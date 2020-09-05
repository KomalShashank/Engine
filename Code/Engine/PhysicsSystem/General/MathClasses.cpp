#include "Engine/PhysicsSystem/General/MathClasses.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Vector2D::Vector2D()
{

}



Vector2D::Vector2D(float initialX, float initialY) :
	X(initialX),
	Y(initialY)
{

}



Vector2D::Vector2D(const Vector2D& copyableVector2D)
{
	X = copyableVector2D.X;
	Y = copyableVector2D.Y;
}



Vector2D Vector2D::operator+(const Vector2D& addableVector2D) const
{
	Vector2D resultantVector2D;
	resultantVector2D.X = X + addableVector2D.X;
	resultantVector2D.Y = Y + addableVector2D.Y;

	return resultantVector2D;
}



Vector2D Vector2D::operator-(const Vector2D& subtractableVector2D) const
{
	Vector2D resultantVector2D;
	resultantVector2D.X = X - subtractableVector2D.X;
	resultantVector2D.Y = Y - subtractableVector2D.Y;

	return resultantVector2D;
}



Vector2D Vector2D::operator*(float multipliableValue) const
{
	Vector2D resultantVector2D;
	resultantVector2D.X = X * multipliableValue;
	resultantVector2D.Y = Y * multipliableValue;

	return resultantVector2D;
}



Vector2D Vector2D::operator/(float dividableValue) const
{
	Vector2D resultantVector2D;
	resultantVector2D.X = X / dividableValue;
	resultantVector2D.Y = Y / dividableValue;

	return resultantVector2D;
}



void Vector2D::operator+=(const Vector2D& addableVector2D)
{
	X += addableVector2D.X;
	Y += addableVector2D.Y;
}



void Vector2D::operator-=(const Vector2D& subtractableVector2D)
{
	X -= subtractableVector2D.X;
	Y -= subtractableVector2D.Y;
}



void Vector2D::operator*=(float multipliableValue)
{
	X *= multipliableValue;
	Y *= multipliableValue;
}



void Vector2D::operator/=(float dividableValue)
{
	X /= dividableValue;
	Y /= dividableValue;
}



bool Vector2D::operator==(const Vector2D& comparableVector2D) const
{
	return ((X == comparableVector2D.X) && (Y == comparableVector2D.Y));
}



bool Vector2D::operator!=(const Vector2D& comparableVector2D) const
{
	return ((X != comparableVector2D.X) || (Y != comparableVector2D.Y));
}



float Vector2D::GetVector2DMagnitude() const
{
	return SquareRoot(GetSquaredVector2DMagnitude());
}



float Vector2D::GetSquaredVector2DMagnitude() const
{
	return ((X * X) + (Y * Y));
}



Vector2D Vector2D::GetNormalizedVector2D() const
{
	float vectorMagnitude = GetVector2DMagnitude();

	Vector2D normalizedVector2D(0.0f, 0.0f);
	if (vectorMagnitude != 0.0f)
	{
		normalizedVector2D.X = X / vectorMagnitude;
		normalizedVector2D.Y = Y / vectorMagnitude;
	}

	return normalizedVector2D;
}



float Vector2D::DotProduct(const Vector2D& firstDottableVector2D, const Vector2D& secondDottableVector2D)
{
	float resultantScalar = (firstDottableVector2D.X * secondDottableVector2D.X) + (firstDottableVector2D.Y * secondDottableVector2D.Y);

	return resultantScalar;
}



float Vector2D::CrossProduct(const Vector2D& firstCrossableVector2D, const Vector2D& secondCrossableVector2D)
{
	return ((firstCrossableVector2D.X * secondCrossableVector2D.Y) - (firstCrossableVector2D.Y * secondCrossableVector2D.X));
}



Vector2D Vector2D::CrossProduct(const Vector2D& crossableVector2D, float crossableScalar)
{
	return Vector2D(crossableScalar * crossableVector2D.Y, -crossableScalar * crossableVector2D.X);
}



Vector2D Vector2D::CrossProduct(float crossableScalar, const Vector2D& crossableVector2D)
{
	return Vector2D(-crossableScalar * crossableVector2D.Y, crossableScalar * crossableVector2D.X);
}



Vector2D Vector2D::GetNegatedVector2D() const
{
	Vector2D negatedVector2D;
	negatedVector2D.X = X * -1.0f;
	negatedVector2D.Y = Y * -1.0f;

	return negatedVector2D;
}



const Vector2D Vector2D::ZERO(0.0f, 0.0f);
const Vector2D Vector2D::ONE(1.0f, 1.0f);
const Vector2D Vector2D::X_AXIS(1.0f, 0.0f);
const Vector2D Vector2D::Y_AXIS(0.0f, 1.0f);



Matrix2x2::Matrix2x2()
{

}



Matrix2x2::Matrix2x2(const float* matrix2x2)
{
	m_Matrix2x2[0] = matrix2x2[0];
	m_Matrix2x2[1] = matrix2x2[1];

	m_Matrix2x2[2] = matrix2x2[2];
	m_Matrix2x2[3] = matrix2x2[3];
}



Matrix2x2::Matrix2x2(const Matrix2x2& copyableMatrix2x2)
{
	m_Matrix2x2[0] = copyableMatrix2x2.m_Matrix2x2[0];
	m_Matrix2x2[1] = copyableMatrix2x2.m_Matrix2x2[1];

	m_Matrix2x2[2] = copyableMatrix2x2.m_Matrix2x2[2];
	m_Matrix2x2[3] = copyableMatrix2x2.m_Matrix2x2[3];
}



Matrix2x2 Matrix2x2::IdentityMatrix2x2()
{
	Matrix2x2 identityMatrix;

	identityMatrix.m_Matrix2x2[0] = 1.0f;
	identityMatrix.m_Matrix2x2[1] = 0.0f;

	identityMatrix.m_Matrix2x2[2] = 0.0f;
	identityMatrix.m_Matrix2x2[3] = 1.0f;

	return identityMatrix;
}



Matrix2x2 Matrix2x2::ZeroMatrix2x2()
{
	Matrix2x2 zeroMatrix;

	zeroMatrix.m_Matrix2x2[0] = 0.0f;
	zeroMatrix.m_Matrix2x2[1] = 0.0f;

	zeroMatrix.m_Matrix2x2[2] = 0.0f;
	zeroMatrix.m_Matrix2x2[3] = 0.0f;

	return zeroMatrix;
}



float Matrix2x2::GetDeterminant() const
{
	float A_11 = m_Matrix2x2[0];
	float A_12 = m_Matrix2x2[1];

	float A_21 = m_Matrix2x2[2];
	float A_22 = m_Matrix2x2[3];

	return (A_11 * A_22) - (A_12 * A_21);
}



float Matrix2x2::GetTrace() const
{
	return (m_Matrix2x2[0] + m_Matrix2x2[3]);
}



bool Matrix2x2::HasInverse(Matrix2x2& outputInverseMatrix2x2) const
{
	float determinant = GetDeterminant();

	if (determinant == 0.0f)
	{
		return false;
	}

	float oneOverDeterminant = 1.0f / determinant;

	outputInverseMatrix2x2.m_Matrix2x2[0] = oneOverDeterminant * m_Matrix2x2[3];
	outputInverseMatrix2x2.m_Matrix2x2[1] = oneOverDeterminant * -m_Matrix2x2[1];

	outputInverseMatrix2x2.m_Matrix2x2[2] = oneOverDeterminant * -m_Matrix2x2[2];
	outputInverseMatrix2x2.m_Matrix2x2[3] = oneOverDeterminant * m_Matrix2x2[0];

	return true;
}



Matrix2x2 Matrix2x2::GetTranspose() const
{
	Matrix2x2 outputTransposeMatrix2x2 = Matrix2x2::IdentityMatrix2x2();
	
	outputTransposeMatrix2x2.m_Matrix2x2[0] = m_Matrix2x2[0];
	outputTransposeMatrix2x2.m_Matrix2x2[1] = m_Matrix2x2[2];

	outputTransposeMatrix2x2.m_Matrix2x2[2] = m_Matrix2x2[1];
	outputTransposeMatrix2x2.m_Matrix2x2[3] = m_Matrix2x2[3];

	return outputTransposeMatrix2x2;
}



Matrix2x2 Matrix2x2::operator+(const Matrix2x2& addableMatrix2x2) const
{
	Matrix2x2 resultantMatrix2x2;

	resultantMatrix2x2.m_Matrix2x2[0] = m_Matrix2x2[0] + addableMatrix2x2.m_Matrix2x2[0];
	resultantMatrix2x2.m_Matrix2x2[1] = m_Matrix2x2[1] + addableMatrix2x2.m_Matrix2x2[1];

	resultantMatrix2x2.m_Matrix2x2[2] = m_Matrix2x2[2] + addableMatrix2x2.m_Matrix2x2[2];
	resultantMatrix2x2.m_Matrix2x2[3] = m_Matrix2x2[3] + addableMatrix2x2.m_Matrix2x2[3];

	return resultantMatrix2x2;
}



Matrix2x2 Matrix2x2::operator-(const Matrix2x2& subtractableMatrix2x2) const
{
	Matrix2x2 resultantMatrix2x2;

	resultantMatrix2x2.m_Matrix2x2[0] = m_Matrix2x2[0] - subtractableMatrix2x2.m_Matrix2x2[0];
	resultantMatrix2x2.m_Matrix2x2[1] = m_Matrix2x2[1] - subtractableMatrix2x2.m_Matrix2x2[1];

	resultantMatrix2x2.m_Matrix2x2[2] = m_Matrix2x2[2] - subtractableMatrix2x2.m_Matrix2x2[2];
	resultantMatrix2x2.m_Matrix2x2[3] = m_Matrix2x2[3] - subtractableMatrix2x2.m_Matrix2x2[3];

	return resultantMatrix2x2;
}



Matrix2x2 Matrix2x2::operator*(float multipliableValue) const
{
	Matrix2x2 resultantMatrix2x2;

	resultantMatrix2x2.m_Matrix2x2[0] = m_Matrix2x2[0] * multipliableValue;
	resultantMatrix2x2.m_Matrix2x2[1] = m_Matrix2x2[1] * multipliableValue;

	resultantMatrix2x2.m_Matrix2x2[2] = m_Matrix2x2[2] * multipliableValue;
	resultantMatrix2x2.m_Matrix2x2[3] = m_Matrix2x2[3] * multipliableValue;

	return resultantMatrix2x2;
}



Matrix2x2 Matrix2x2::operator/(float dividableValue) const
{
	Matrix2x2 resultantMatrix2x2;

	resultantMatrix2x2.m_Matrix2x2[0] = m_Matrix2x2[0] / dividableValue;
	resultantMatrix2x2.m_Matrix2x2[1] = m_Matrix2x2[1] / dividableValue;

	resultantMatrix2x2.m_Matrix2x2[2] = m_Matrix2x2[2] / dividableValue;
	resultantMatrix2x2.m_Matrix2x2[3] = m_Matrix2x2[3] / dividableValue;

	return resultantMatrix2x2;
}



void Matrix2x2::operator+=(const Matrix2x2& addableMatrix2x2)
{
	m_Matrix2x2[0] += addableMatrix2x2.m_Matrix2x2[0];
	m_Matrix2x2[1] += addableMatrix2x2.m_Matrix2x2[1];

	m_Matrix2x2[2] += addableMatrix2x2.m_Matrix2x2[2];
	m_Matrix2x2[3] += addableMatrix2x2.m_Matrix2x2[3];
}



void Matrix2x2::operator-=(const Matrix2x2& subtractableMatrix2x2)
{
	m_Matrix2x2[0] -= subtractableMatrix2x2.m_Matrix2x2[0];
	m_Matrix2x2[1] -= subtractableMatrix2x2.m_Matrix2x2[1];

	m_Matrix2x2[2] -= subtractableMatrix2x2.m_Matrix2x2[2];
	m_Matrix2x2[3] -= subtractableMatrix2x2.m_Matrix2x2[3];
}



void Matrix2x2::operator*=(float multipliableValue)
{
	m_Matrix2x2[0] *= multipliableValue;
	m_Matrix2x2[1] *= multipliableValue;

	m_Matrix2x2[2] *= multipliableValue;
	m_Matrix2x2[3] *= multipliableValue;
}



void Matrix2x2::operator/=(float dividableValue)
{
	m_Matrix2x2[0] /= dividableValue;
	m_Matrix2x2[1] /= dividableValue;

	m_Matrix2x2[2] /= dividableValue;
	m_Matrix2x2[3] /= dividableValue;
}



bool Matrix2x2::operator==(const Matrix2x2& comparableMatrix2x2) const
{
	return (m_Matrix2x2[0] == comparableMatrix2x2.m_Matrix2x2[0] &&
		m_Matrix2x2[1] == comparableMatrix2x2.m_Matrix2x2[1] &&

		m_Matrix2x2[2] == comparableMatrix2x2.m_Matrix2x2[2] &&
		m_Matrix2x2[3] == comparableMatrix2x2.m_Matrix2x2[3]);
}



bool Matrix2x2::operator!=(const Matrix2x2& comparableMatrix2x2) const
{
	return (m_Matrix2x2[0] != comparableMatrix2x2.m_Matrix2x2[0] ||
		m_Matrix2x2[1] != comparableMatrix2x2.m_Matrix2x2[1] ||

		m_Matrix2x2[2] != comparableMatrix2x2.m_Matrix2x2[2] ||
		m_Matrix2x2[3] != comparableMatrix2x2.m_Matrix2x2[3]);
}



bool Matrix2x2::IsSymmetric() const
{
	Matrix2x2 transposeMatrix2x2 = GetTranspose();

	if ((*this) == transposeMatrix2x2)
	{
		return true;
	}

	return false;
}



bool Matrix2x2::IsSkewSymmetric() const
{
	Matrix2x2 transposeMatrix2x2 = GetTranspose();

	Matrix2x2 negatedTransposeMatrix2x2 = transposeMatrix2x2 * -1.0f;

	if ((*this) == negatedTransposeMatrix2x2)
	{
		return true;
	}

	return false;
}



bool Matrix2x2::IsOrthogonal() const
{
	Matrix2x2 inverseMatrix2x2;

	if (HasInverse(inverseMatrix2x2))
	{
		Matrix2x2 transposeMatrix2x2 = GetTranspose();

		if (inverseMatrix2x2 == transposeMatrix2x2)
		{
			return true;
		}
	}

	return false;
}



AABB2D::AABB2D()
{

}



AABB2D::AABB2D(const Vector2D& initialMinimums, const Vector2D& initialMaximums) :
	minimums(initialMinimums),
	maximums(initialMaximums)
{

}



AABB2D::AABB2D(const AABB2D& copyableAABB)
{
	minimums = copyableAABB.minimums;
	maximums = copyableAABB.maximums;
}



Vector2D AABB2D::GetAABBCenter() const
{
	return ((minimums + maximums) * 0.5f);
}



Vector2D AABB2D::GetHalfDimensions() const
{
	return ((maximums - minimums) * 0.5f);
}



float AABB2D::GetAABBPerimeter() const
{
	Vector2D halfDimensions = GetHalfDimensions();

	return (4.0f * (halfDimensions.X + halfDimensions.Y));
}



bool AABB2D::ContainsAABB(const AABB2D& containableAABB) const
{
	bool result = true;

	result &= (minimums.X <= containableAABB.minimums.X);
	result &= (minimums.Y <= containableAABB.minimums.Y);
	result &= (containableAABB.maximums.X <= maximums.X);
	result &= (containableAABB.maximums.Y <= maximums.Y);

	return result;
}



void AABB2D::CombineAABBs(const AABB2D& combinableAABB)
{
	minimums = GetMinimum(minimums, combinableAABB.minimums);
	maximums = GetMaximum(maximums, combinableAABB.maximums);
}



AABB2D AABB2D::GetCombinedAABB(const AABB2D& firstCombinableAABB, const AABB2D& secondCombinableAABB)
{
	AABB2D combinedAABB = firstCombinableAABB;
	combinedAABB.CombineAABBs(secondCombinableAABB);

	return combinedAABB;
}



bool AABB2D::AreAABBsIntersecting(const AABB2D& firstAABB, const AABB2D& secondAABB)
{
// 	if (firstAABB.maximums.X < secondAABB.minimums.X)
// 	{
// 		return false;
// 	}
// 
// 	if (firstAABB.maximums.Y < secondAABB.minimums.Y)
// 	{
// 		return false;
// 	}
// 
// 	if (secondAABB.maximums.X < firstAABB.minimums.X)
// 	{
// 		return false;
// 	}
// 
// 	if (secondAABB.maximums.Y < firstAABB.minimums.Y)
// 	{
// 		return false;
// 	}
// 
// 	return true;

	Vector2D firstDifference = secondAABB.minimums - firstAABB.maximums;
	Vector2D secondDifference = firstAABB.minimums - secondAABB.maximums;

	if (firstDifference.X > 0.0f || firstDifference.Y > 0.0f)
	{
		return false;
	}

	if (secondDifference.X > 0.0f || secondDifference.Y > 0.0f)
	{
		return false;
	}

	return true;
}



Rotator2D::Rotator2D() :
	m_SinAngle(0.0f),
	m_CosAngle(1.0f)
{

}



Rotator2D::Rotator2D(float angleInDegrees) :
	m_SinAngle(SineOfDegrees(angleInDegrees)),
	m_CosAngle(CosineOfDegrees(angleInDegrees))
{

}



void Rotator2D::SetIdentityRotator2D()
{
	m_SinAngle = 0.0f;
	m_CosAngle = 1.0f;
}



void Rotator2D::SetRotator2D(float angleInDegrees)
{
	m_SinAngle = SineOfDegrees(angleInDegrees);
	m_CosAngle = CosineOfDegrees(angleInDegrees);
}



float Rotator2D::GetAngle() const
{
	return ArcTangentInDegrees(Vector2(m_CosAngle, m_SinAngle));
}



Transform2D::Transform2D() :
	m_Position(Vector2D::ZERO)
{
	m_Rotation.SetIdentityRotator2D();
}



Transform2D::Transform2D(const Vector2D& transformPosition, float transformRotation) :
	m_Position(transformPosition)
{
	m_Rotation.SetRotator2D(transformRotation);
}



void Transform2D::SetIdentityTransform2D()
{
	m_Position = Vector2D::ZERO;
	m_Rotation.SetIdentityRotator2D();
}



void Transform2D::SetTransform2D(const Vector2D& transformPosition, float transformRotation)
{
	m_Position = transformPosition;
	m_Rotation.SetRotator2D(transformRotation);
}



float SquareRootOfValue(float value)
{
	return SquareRoot(value);
}



float FloorOfValue(float value)
{
	return RoundDownToFloorValue(value);
}



Vector2D Multiply(const Transform2D& transform2D, const Vector2D& vector2D)
{
	Vector2D rotatedVector2D = Multiply(transform2D.m_Rotation, vector2D);

	return (rotatedVector2D + transform2D.m_Position);
}



Vector2D MultiplyTranspose(const Transform2D& transform2D, const Vector2D& vector2D)
{
	Vector2D translatedVector2D = vector2D - transform2D.m_Position;

	return MultiplyTranspose(transform2D.m_Rotation, translatedVector2D);
}



Vector2D Multiply(const Rotator2D& rotation2D, const Vector2D& vector2D)
{
	float X = (rotation2D.m_CosAngle * vector2D.X) - (rotation2D.m_SinAngle * vector2D.Y);
	float Y = (rotation2D.m_SinAngle * vector2D.X) + (rotation2D.m_CosAngle * vector2D.Y);

	return Vector2D(X, Y);
}



Vector2D MultiplyTranspose(const Rotator2D& rotation2D, const Vector2D& vector2D)
{
	float X = (rotation2D.m_CosAngle * vector2D.X) + (rotation2D.m_SinAngle * vector2D.Y);
	float Y = (-rotation2D.m_SinAngle * vector2D.X) + (rotation2D.m_CosAngle * vector2D.Y);

	return Vector2D(X, Y);
}