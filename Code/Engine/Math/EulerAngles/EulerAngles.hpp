#pragma once



class EulerAngles
{
public:
	EulerAngles();
	EulerAngles(float rollAngleInDegrees, float pitchAngleInDegrees, float yawAngleInDegrees);

public:
	static const EulerAngles ZERO;

public:
	float m_RollAngleInDegrees;
	float m_PitchAngleInDegrees;
	float m_YawAngleInDegrees;
};