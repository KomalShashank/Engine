#include "Engine/Math/EulerAngles/EulerAngles.hpp"



const EulerAngles EulerAngles::ZERO(0.0f, 0.0f, 0.0f);



EulerAngles::EulerAngles()
{

}



EulerAngles::EulerAngles(float rollAngleInDegrees, float pitchAngleInDegrees, float yawAngleInDegrees) :
m_RollAngleInDegrees(rollAngleInDegrees),
m_PitchAngleInDegrees(pitchAngleInDegrees),
m_YawAngleInDegrees(yawAngleInDegrees)
{

}