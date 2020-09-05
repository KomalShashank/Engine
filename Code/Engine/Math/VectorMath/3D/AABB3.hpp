#pragma once

#include "Engine/Math/VectorMath/3D/Vector3.hpp"



class AABB3
{
public:
	AABB3();
	AABB3(const Vector3& initialMinimums, const Vector3& initialMaximums);
	AABB3(const AABB3& copyableAABB);

public:
	static const AABB3 UNIT_AABB3;

public:
	Vector3 minimums;
	Vector3 maximums;
};