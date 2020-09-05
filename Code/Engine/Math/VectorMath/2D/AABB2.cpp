#include "Engine/Math/VectorMath/2D/AABB2.hpp"



AABB2::AABB2()
{

}



AABB2::AABB2(const Vector2& initialMinimums, const Vector2& initialMaximums) : minimums(initialMinimums), maximums(initialMaximums)
{

}



AABB2::AABB2(const AABB2& copyableAABB)
{
	minimums = copyableAABB.minimums;
	maximums = copyableAABB.maximums;
}



const AABB2 AABB2::UNIT_AABB2(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));