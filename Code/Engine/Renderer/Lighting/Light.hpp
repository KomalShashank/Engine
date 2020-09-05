#pragma once

#include "Engine/Math/VectorMath/3D/Vector3.hpp"



const size_t MAXIMUM_NUMBER_OF_LIGHTS = 16U;



class LightDistanceData
{
public:
	LightDistanceData() :
		m_NearDistance(1.0f),
		m_FarDistance(1.0f),
		m_NearFactor(1.0f),
		m_FarFactor(1.0f),
		m_Used(false)
	{

	}
	
public:
	float m_NearDistance;
	float m_FarDistance;
	float m_NearFactor;
	float m_FarFactor;

	bool m_Used;
};



class LightDirectionData
{
public:
	LightDirectionData() :
		m_Direction(Vector3::ZERO),
		m_DirectionalFactor(0.0f),
		m_Used(false)
	{

	}

public:
	Vector3 m_Direction;
	float m_DirectionalFactor;

	bool m_Used;
};



class LightFocalData
{
public:
	LightFocalData() :
		m_InnerAngle(1.0f),
		m_OuterAngle(-1.0f),
		m_InnerFactor(1.0f),
		m_OuterFactor(1.0f),
		m_Used(false)
	{

	}

public:
	float m_InnerAngle;
	float m_OuterAngle;
	float m_InnerFactor;
	float m_OuterFactor;

	bool m_Used;
};



class Light
{
private:
	Light(bool distanceDataUsed = false, bool directionDataUsed = false, bool focalDataUsed = false);

public:
	void SetLightPosition(const Vector3& lightPosition);
	Vector3 GetLightPosition() const;

	void SetLightColor(const Vector3& lightColor);
	Vector3 GetLightColor() const;

	void SetNearDistance(float nearDistance);
	float GetNearDistance() const;

	void SetFarDistance(float farDistance);
	float GetFarDistance() const;

	void SetNearFactor(float nearFactor);
	float GetNearFactor() const;

	void SetFarFactor(float farFactor);
	float GetFarFactor() const;

	void SetLightDirection(const Vector3& lightDirection);
	Vector3 GetLightDirection() const;

	void SetDirectionalFactor(float directionalFactor);
	float GetDirectionalFactor() const;

	void SetInnerAngle(float innerAngle);
	float GetInnerAngle() const;

	void SetOuterAngle(float outerAngle);
	float GetOuterAngle() const;

	void SetInnerFactor(float innerFactor);
	float GetInnerFactor() const;

	void SetOuterFactor(float outerFactor);
	float GetOuterFactor() const;

private:
	Vector3 m_Position;
	Vector3 m_Color;

	LightDistanceData m_LightDistanceData;
	LightDirectionData m_LightDirectionData;
	LightFocalData m_LightFocalData;

public:
	static const Light LOCAL_POINT_LIGHT;
	static const Light GLOBAL_POINT_LIGHT;
	static const Light LOCAL_SPOT_LIGHT;
	static const Light GLOBAL_SPOT_LIGHT;
	static const Light GLOBAL_DIRECTIONAL_LIGHT;
};