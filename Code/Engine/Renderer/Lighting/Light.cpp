#include "Engine/Renderer/Lighting/Light.hpp"



Light::Light(bool distanceDataUsed /*= false*/, bool directionDataUsed /*= false*/, bool focalDataUsed /*= false*/) :
	m_Position(Vector3::ZERO),
	m_Color(Vector3::ZERO)
{
	m_LightDistanceData.m_Used = distanceDataUsed;
	m_LightDirectionData.m_Used = directionDataUsed;
	m_LightFocalData.m_Used = focalDataUsed;
}



void Light::SetLightPosition(const Vector3& lightPosition)
{
	m_Position = lightPosition;
}



Vector3 Light::GetLightPosition() const
{
	return m_Position;
}



void Light::SetLightColor(const Vector3& lightColor)
{
	m_Color = lightColor;
}



Vector3 Light::GetLightColor() const
{
	return m_Color;
}



void Light::SetNearDistance(float nearDistance)
{
	if (m_LightDistanceData.m_Used)
	{
		m_LightDistanceData.m_NearDistance = nearDistance;
	}
}



float Light::GetNearDistance() const
{
	return m_LightDistanceData.m_NearDistance;
}



void Light::SetFarDistance(float farDistance)
{
	if (m_LightDistanceData.m_Used)
	{
		m_LightDistanceData.m_FarDistance = farDistance;
	}
}



float Light::GetFarDistance() const
{
	return m_LightDistanceData.m_FarDistance;
}



void Light::SetNearFactor(float nearFactor)
{
	if (m_LightDistanceData.m_Used)
	{
		m_LightDistanceData.m_NearFactor = nearFactor;
	}
}



float Light::GetNearFactor() const
{
	return m_LightDistanceData.m_NearFactor;
}



void Light::SetFarFactor(float farFactor)
{
	if (m_LightDistanceData.m_Used)
	{
		m_LightDistanceData.m_FarFactor = farFactor;
	}
}



float Light::GetFarFactor() const
{
	return m_LightDistanceData.m_FarFactor;
}



void Light::SetLightDirection(const Vector3& lightDirection)
{
	if (m_LightDirectionData.m_Used)
	{
		m_LightDirectionData.m_Direction = lightDirection;
	}
}



Vector3 Light::GetLightDirection() const
{
	return m_LightDirectionData.m_Direction;
}



void Light::SetDirectionalFactor(float directionalFactor)
{
	if (m_LightDirectionData.m_Used)
	{
		m_LightDirectionData.m_DirectionalFactor = directionalFactor;
	}
}



float Light::GetDirectionalFactor() const
{
	return m_LightDirectionData.m_DirectionalFactor;
}



void Light::SetInnerAngle(float innerAngle)
{
	if (m_LightFocalData.m_Used)
	{
		m_LightFocalData.m_InnerAngle = innerAngle;
	}
}



float Light::GetInnerAngle() const
{
	return m_LightFocalData.m_InnerAngle;
}



void Light::SetOuterAngle(float outerAngle)
{
	if (m_LightFocalData.m_Used)
	{
		m_LightFocalData.m_OuterAngle = outerAngle;
	}
}



float Light::GetOuterAngle() const
{
	return m_LightFocalData.m_OuterAngle;
}



void Light::SetInnerFactor(float innerFactor)
{
	if (m_LightFocalData.m_Used)
	{
		m_LightFocalData.m_InnerFactor = innerFactor;
	}
}



float Light::GetInnerFactor() const
{
	return m_LightFocalData.m_InnerFactor;
}



void Light::SetOuterFactor(float outerFactor)
{
	if (m_LightFocalData.m_Used)
	{
		m_LightFocalData.m_OuterFactor = outerFactor;
	}
}



float Light::GetOuterFactor() const
{
	return m_LightFocalData.m_OuterFactor;
}



const Light Light::LOCAL_POINT_LIGHT(true, false, false);
const Light Light::GLOBAL_POINT_LIGHT(false, false, false);
const Light Light::LOCAL_SPOT_LIGHT(true, true, true);
const Light Light::GLOBAL_SPOT_LIGHT(false, true, true);
const Light Light::GLOBAL_DIRECTIONAL_LIGHT(false, true, false);