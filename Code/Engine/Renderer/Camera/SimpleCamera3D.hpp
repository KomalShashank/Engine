#pragma once

#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/EulerAngles/EulerAngles.hpp"



class Camera3D
{
public:
	Camera3D();
	Camera3D(const Vector3& position, const EulerAngles& orientation);
	void FixAndClampAngles();
	Vector3 GetForwardXYZ() const;
	Vector3 GetForwardXY() const;
	Vector3 GetLeftXY() const;

public:
	Vector3 m_Position;
	EulerAngles m_Orientation;
};