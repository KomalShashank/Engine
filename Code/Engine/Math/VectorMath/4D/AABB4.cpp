#include "Engine/Math/VectorMath/4D/AABB4.hpp"



AABB4::AABB4()
{

}



AABB4::AABB4(const Vector4& initialMinimums, const Vector4& initialMaximums) : minimums(initialMinimums), maximums(initialMaximums)
{

}



AABB4::AABB4(const AABB4& copyableAABB)
{
	minimums = copyableAABB.minimums;
	maximums = copyableAABB.maximums;
}

const AABB4 AABB4::UNIT_AABB4(Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));