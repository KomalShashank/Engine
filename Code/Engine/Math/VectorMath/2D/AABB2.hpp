#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"



class AABB2
{
public:
	AABB2();
	AABB2(const Vector2& initialMinimums, const Vector2& initialMaximums);
	AABB2(const AABB2& copyableAABB);

public:
	static const AABB2 UNIT_AABB2;

public:
	Vector2 minimums;
	Vector2 maximums;
};