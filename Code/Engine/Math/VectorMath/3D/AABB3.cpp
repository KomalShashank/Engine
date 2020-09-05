#include "Engine/Math/VectorMath/3D/AABB3.hpp"



AABB3::AABB3()
{

}



AABB3::AABB3(const Vector3& initialMinimums, const Vector3& initialMaximums) : minimums(initialMinimums), maximums(initialMaximums)
{

}



AABB3::AABB3(const AABB3& copyableAABB)
{
	minimums = copyableAABB.minimums;
	maximums = copyableAABB.maximums;
}

const AABB3 AABB3::UNIT_AABB3(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));