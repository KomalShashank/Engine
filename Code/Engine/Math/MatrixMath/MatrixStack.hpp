#pragma once

#include <stack>

#include "Engine/Math/MatrixMath/Matrix2.hpp"
#include "Engine/Math/MatrixMath/Matrix3.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"



class MatrixStack
{
public:
	MatrixStack();

	bool IsEmpty();
	Matrix4 GetTop();
	void PushToMatrixStack(const Matrix4& newMatrix);
	void PopFromMatrixStack();

public:
	std::stack<Matrix4> m_Matrices;
};