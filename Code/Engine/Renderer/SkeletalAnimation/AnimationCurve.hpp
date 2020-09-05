#pragma once

#include <vector>
#include "Engine/Math/MatrixMath/Matrix4.hpp"



enum InterpolationType
{
	INVALID_INTERPOLATION_TYPE = -1,
	CONSTANT_INTERPOLATION,
	LINEAR_INTERPOLATION,
	CUBIC_INTERPOLATION
};



struct KeyFrame
{
	float frameCurrentTime;
	float frameValue;
	float negativeSlope;
	float positiveSlope;
	InterpolationType currentInterpolationType;
};



class JointInfo
{
public:
	JointInfo();
	Matrix4 CalculateLocalTransform(const Matrix4& scalingMatrix, const Matrix4& rotationMatrix, const Matrix4& translationMatrix);

public:
	Vector3 scalePivot;
	Vector3 scaleOffset;
	Vector3 rotationPivot;
	Vector3 rotationOffset;
	Matrix4 preRotationMatrix;
	Matrix4 postRotationMatrix;
};



class TransformCurve
{
public:
	TransformCurve();

	float GetFrameValueAtTime(float currentTime);
	void GetClosestKeyFramesForTime(float currentTime, KeyFrame& firstKeyFrame, KeyFrame& secondKeyFrame, float& blendFactor);

	void InsertKeyFrame(const KeyFrame& currentKeyFrame);

public:
	std::vector<KeyFrame> m_KeyFrames;
};



class AnimationCurve
{
public:
	AnimationCurve();

	Matrix4 GetLocalTransformAtTime(float currentTime, JointInfo* currentJointInfo);

public:
	TransformCurve m_ScalingCurve[3];
	TransformCurve m_RotationCurve[3];
	TransformCurve m_TranslationCurve[3];
};