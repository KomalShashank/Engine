#include "Engine/Renderer/Camera/SimpleCamera3D.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Camera3D::Camera3D()
{

}



Camera3D::Camera3D(const Vector3& position, const EulerAngles& orientation) : m_Position(position), m_Orientation(orientation)
{

}



void Camera3D::FixAndClampAngles()
{
	m_Orientation.m_YawAngleInDegrees = WrapAroundCircularRange(m_Orientation.m_YawAngleInDegrees, 0.0f, 360.0f);
	m_Orientation.m_PitchAngleInDegrees = ClampFloat(m_Orientation.m_PitchAngleInDegrees, -89.9f, 89.9f);
	m_Orientation.m_RollAngleInDegrees = ClampFloat(m_Orientation.m_RollAngleInDegrees, -89.9f, 89.9f);
}



Vector3 Camera3D::GetForwardXYZ() const
{
	Vector3 forwardXY = GetForwardXY();

	float cosineOfPitchAngle = CosineOfDegrees(m_Orientation.m_PitchAngleInDegrees);
	float sineOfPitchAngle = SineOfDegrees(m_Orientation.m_PitchAngleInDegrees);

	Vector3 forwardXYZ = Vector3(forwardXY.X * cosineOfPitchAngle, forwardXY.Y * cosineOfPitchAngle, -sineOfPitchAngle);

	return forwardXYZ;
}



Vector3 Camera3D::GetForwardXY() const
{
	Vector3 forwardXY;

	float cosineOfYawAngle = CosineOfDegrees(m_Orientation.m_YawAngleInDegrees);
	float sineOfYawAngle = SineOfDegrees(m_Orientation.m_YawAngleInDegrees);

	forwardXY = Vector3(cosineOfYawAngle, sineOfYawAngle, 0.0f);

	return forwardXY;
}



Vector3 Camera3D::GetLeftXY() const
{
	Vector3 forwardXY = GetForwardXY();
	Vector3 leftXY = Vector3(-forwardXY.Y, forwardXY.X, 0.0f);

	return leftXY;
}