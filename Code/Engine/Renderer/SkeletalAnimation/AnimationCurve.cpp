#include "Engine/Renderer/SkeletalAnimation/AnimationCurve.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



JointInfo::JointInfo()
{

}



Matrix4 JointInfo::CalculateLocalTransform(const Matrix4& scalingMatrix, const Matrix4& rotationMatrix, const Matrix4& translationMatrix)
{
	Matrix4 scaleOffsetMatrix = Matrix4::SetTranslationMatrix4(scaleOffset);
	Matrix4 scalePivotMatrix = Matrix4::SetTranslationMatrix4(scalePivot);
	Matrix4 scalePivotInverseMatrix = Matrix4::SetTranslationMatrix4(scalePivot.GetNegatedVector3());

	Matrix4 rotationOffsetMatrix = Matrix4::SetTranslationMatrix4(rotationOffset);
	Matrix4 rotationPivotMatrix = Matrix4::SetTranslationMatrix4(rotationPivot);
	Matrix4 rotationPivotInverseMatrix = Matrix4::SetTranslationMatrix4(rotationPivot.GetNegatedVector3());

	Matrix4 finalLocalTransform =	scalePivotInverseMatrix *
									scalingMatrix *
									scalePivotMatrix *
									scaleOffsetMatrix *
									rotationPivotInverseMatrix *
									postRotationMatrix *
									rotationMatrix *
									preRotationMatrix *
									rotationPivotMatrix *
									rotationOffsetMatrix *
									translationMatrix;

	return finalLocalTransform;
}



TransformCurve::TransformCurve()
{

}



float TransformCurve::GetFrameValueAtTime(float currentTime)
{
	KeyFrame firstKeyFrame;
	KeyFrame secondKeyFrame;
	float blendFactor;

	GetClosestKeyFramesForTime(currentTime, firstKeyFrame, secondKeyFrame, blendFactor);

	switch (firstKeyFrame.currentInterpolationType)
	{
	case CONSTANT_INTERPOLATION:
		return firstKeyFrame.frameValue;

	case LINEAR_INTERPOLATION:
		return LinearlyInterpolateIn1D(firstKeyFrame.frameValue, secondKeyFrame.frameValue, blendFactor);

	case CUBIC_INTERPOLATION:
		return CardinallyInterpolateIn2D(firstKeyFrame.frameValue, secondKeyFrame.frameValue, firstKeyFrame.negativeSlope, firstKeyFrame.positiveSlope, blendFactor);

	default:
		return -1.0f;
	}
}



void TransformCurve::GetClosestKeyFramesForTime(float currentTime, KeyFrame& firstKeyFrame, KeyFrame& secondKeyFrame, float& blendFactor)
{
	for (size_t frameIndex = 0; frameIndex < m_KeyFrames.size(); ++frameIndex)
	{
		if (m_KeyFrames[frameIndex].frameCurrentTime == currentTime)
		{
			firstKeyFrame = m_KeyFrames[frameIndex];
			secondKeyFrame = m_KeyFrames[frameIndex];
			blendFactor = 0.0f;

			return;
		}
		else if (m_KeyFrames[frameIndex].frameCurrentTime > currentTime)
		{
			secondKeyFrame = m_KeyFrames[frameIndex];

			if (frameIndex == 0)
			{
				firstKeyFrame = m_KeyFrames[frameIndex];
				blendFactor = 0.0f;
			}
			else
			{
				firstKeyFrame = m_KeyFrames[frameIndex - 1];
				blendFactor = RangeMap(currentTime, firstKeyFrame.frameCurrentTime, secondKeyFrame.frameCurrentTime, 0.0f, 1.0f);
			}

			return;
		}
	}

	firstKeyFrame = m_KeyFrames[0];
	secondKeyFrame = m_KeyFrames[0];
	blendFactor = 0.0f;
}



void TransformCurve::InsertKeyFrame(const KeyFrame& currentKeyFrame)
{
	m_KeyFrames.push_back(currentKeyFrame);
}



AnimationCurve::AnimationCurve()
{

}



Matrix4 AnimationCurve::GetLocalTransformAtTime(float currentTime, JointInfo* currentJointInfo)
{
	Matrix4 currentTranslation = Matrix4::IdentityMatrix4();

	currentTranslation.m_Matrix4[12] = m_TranslationCurve[0].GetFrameValueAtTime(currentTime);
	currentTranslation.m_Matrix4[13] = m_TranslationCurve[1].GetFrameValueAtTime(currentTime);
	currentTranslation.m_Matrix4[14] = m_TranslationCurve[2].GetFrameValueAtTime(currentTime);

	Matrix4 currentXRotation = Matrix4::IdentityMatrix4();
	Matrix4 currentYRotation = Matrix4::IdentityMatrix4();
	Matrix4 currentZRotation = Matrix4::IdentityMatrix4();

	float angleX = m_RotationCurve[0].GetFrameValueAtTime(currentTime);
	float cosX = CosineOfDegrees(angleX);
	float sinX = SineOfDegrees(angleX);

	currentXRotation.m_Matrix4[5] = cosX;
	currentXRotation.m_Matrix4[10] = cosX;
	currentXRotation.m_Matrix4[6] = -sinX;
	currentXRotation.m_Matrix4[9] = sinX;

	float angleY = m_RotationCurve[1].GetFrameValueAtTime(currentTime);
	float cosY = CosineOfDegrees(angleY);
	float sinY = SineOfDegrees(angleY);

	currentYRotation.m_Matrix4[0] = cosY;
	currentYRotation.m_Matrix4[10] = cosY;
	currentYRotation.m_Matrix4[2] = -sinY;
	currentYRotation.m_Matrix4[8] = sinY;

	float angleZ = m_RotationCurve[2].GetFrameValueAtTime(currentTime);
	float cosZ = CosineOfDegrees(angleZ);
	float sinZ = SineOfDegrees(angleZ);

	currentZRotation.m_Matrix4[0] = cosZ;
	currentZRotation.m_Matrix4[5] = cosZ;
	currentZRotation.m_Matrix4[1] = -sinZ;
	currentZRotation.m_Matrix4[4] = sinZ;

	Matrix4 currentYRotationInverse;
	currentYRotation.HasInverse(currentYRotationInverse);

	Matrix4 currentRotation = Matrix4::IdentityMatrix4();
	currentRotation = currentXRotation * currentYRotationInverse * currentZRotation;

	Matrix4 currentScaling = Matrix4::IdentityMatrix4();

	currentScaling.m_Matrix4[0] = m_ScalingCurve[0].GetFrameValueAtTime(currentTime);
	currentScaling.m_Matrix4[5] = m_ScalingCurve[1].GetFrameValueAtTime(currentTime);
	currentScaling.m_Matrix4[10] = m_ScalingCurve[2].GetFrameValueAtTime(currentTime);

	Matrix4 localTransform = currentJointInfo->CalculateLocalTransform(currentScaling, currentRotation, currentTranslation);

	return localTransform;
}