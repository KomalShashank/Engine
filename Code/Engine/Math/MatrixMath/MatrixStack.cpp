#include "Engine/Math/MatrixMath/MatrixStack.hpp"



MatrixStack::MatrixStack()
{
	m_Matrices.push(Matrix4::IdentityMatrix4());
}



bool MatrixStack::IsEmpty()
{
	return (m_Matrices.size() == 1);
}



Matrix4 MatrixStack::GetTop()
{
	return m_Matrices.top();
}



void MatrixStack::PushToMatrixStack(const Matrix4& newMatrix)
{
	Matrix4 topMatrix = GetTop();
	Matrix4 newTopMatrix = Matrix4::Multiply(newMatrix, topMatrix);
	m_Matrices.push(newTopMatrix);
}



void MatrixStack::PopFromMatrixStack()
{
	if (!IsEmpty())
	{
		m_Matrices.pop();
	}
}