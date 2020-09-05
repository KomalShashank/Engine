#pragma once

#include "Engine/Math/VectorMath/4D/Vector4.hpp"



class AABB4
{
public:
	AABB4();
	AABB4(const Vector4& initialMinimums, const Vector4& initialMaximums);
	AABB4(const AABB4& copyableAABB);

public:
	static const AABB4 UNIT_AABB4;

public:
	Vector4 minimums;
	Vector4 maximums;
};