#include "Engine/Renderer/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



SkeletalAnimation::SkeletalAnimation() :
m_AnimationName(""),
m_NumberOfFrames(0),
m_NumberOfSkeletonJoints(0),
m_DurationInSeconds(0.0f),
m_FrameRate(0.0f),
m_FrameTime(0.0f)
{

}



SkeletalAnimation::SkeletalAnimation(const std::string& animationName, float startingTime, float durationInSeconds, float frameRate, MeshSkeleton* skeleton) :
m_AnimationName(animationName),
m_StartingTime(startingTime),
m_DurationInSeconds(durationInSeconds),
m_FrameRate(frameRate),
m_FrameTime(1.0f / frameRate)
{
	m_NumberOfFrames = static_cast<uint32_t>(RoundUpToCeilingValue(frameRate * durationInSeconds)) + 1;
	m_NumberOfSkeletonJoints = skeleton->GetNumberOfJoints();

	uint32_t totalNumberOfKeyFrames = m_NumberOfFrames * m_NumberOfSkeletonJoints;
	m_KeyFrames.resize(totalNumberOfKeyFrames);
}



Matrix4* SkeletalAnimation::GetKeyFramesForJointIndex(uint32_t jointIndex)
{
	return &m_KeyFrames[m_NumberOfFrames * jointIndex];
}



void SkeletalAnimation::GetFrameIndicesAndBlendFactor(uint32_t& startingFrameIndex, uint32_t& endingFrameIndex, float& blendFactor, float animationTime)
{
	uint32_t firstFrameIndex = static_cast<uint32_t>(RoundDownToFloorValue(animationTime / m_FrameTime));
	uint32_t secondFrameIndex = firstFrameIndex + 1;

	if (firstFrameIndex == (m_NumberOfFrames - 1))
	{
		secondFrameIndex = m_NumberOfFrames - 1;
		blendFactor = 0.0f;
	}
	else if (firstFrameIndex == (m_NumberOfFrames - 2))
	{
		float lastFrameTime = m_DurationInSeconds - (m_FrameTime * firstFrameIndex);
		blendFactor = CalculateModulus(animationTime, m_FrameTime) / lastFrameTime;
		blendFactor = ClampFloat(blendFactor, 0.0f, 1.0f);
	}
	else
	{
		blendFactor = CalculateModulus(animationTime, m_FrameTime) / m_FrameTime;
	}

	startingFrameIndex = firstFrameIndex;
	endingFrameIndex = secondFrameIndex;
}



uint32_t SkeletalAnimation::GetNumberOfFrames() const
{
	return m_NumberOfFrames;
}



float SkeletalAnimation::GetStartingTime() const
{
	return m_StartingTime;
}



float SkeletalAnimation::GetAnimationDuration() const
{
	return m_DurationInSeconds;
}



float SkeletalAnimation::GetFrameRate() const
{
	return m_FrameRate;
}



float SkeletalAnimation::GetFrameTime() const
{
	return m_FrameTime;
}



void SkeletalAnimation::InsertAnimationCurve(const AnimationCurve& animationCurve)
{
	m_AnimationCurves.push_back(animationCurve);
}



void SkeletalAnimation::ApplyKeyFrameAnimation(MeshSkeleton* skeleton, float currentAnimationTime, const SkeletalAnimationMode& animationMode)
{
	uint32_t startingFrameIndex;
	uint32_t endingFrameIndex;
	float blendFactor;

	float animationTime = GetAnimationTimeForMode(currentAnimationTime, animationMode);

	GetFrameIndicesAndBlendFactor(startingFrameIndex, endingFrameIndex, blendFactor, animationTime);

	for (uint32_t jointIndex = 0; jointIndex < m_NumberOfSkeletonJoints; ++jointIndex)
	{
		Matrix4* jointKeyFrames = GetKeyFramesForJointIndex(jointIndex);
		Matrix4& startingKeyFrame = jointKeyFrames[startingFrameIndex];
		Matrix4& endingKeyFrame = jointKeyFrames[endingFrameIndex];

		Matrix4 transformedModelMatrix = Matrix4::InterpolateMatrices4(startingKeyFrame, endingKeyFrame, blendFactor);
		skeleton->SetJointGlobalTransform(jointIndex, transformedModelMatrix);
	}
}



void SkeletalAnimation::ApplyCurveAnimation(MeshSkeleton* skeleton, float currentAnimationTime, const SkeletalAnimationMode& animationMode)
{
	float animationTime = GetAnimationTimeForMode(currentAnimationTime, animationMode);
	
	for (uint32_t jointIndex = 0; jointIndex < m_NumberOfSkeletonJoints; ++jointIndex)
	{
		AnimationCurve currentAnimationCurve = m_AnimationCurves[jointIndex];
		JointInfo* currentJointInfo = skeleton->GetJointInfoByIndex(jointIndex);

		Matrix4 currentLocalTransform = currentAnimationCurve.GetLocalTransformAtTime(animationTime, currentJointInfo);
		skeleton->SetJointLocalTransform(jointIndex, currentLocalTransform);
	}
}



void SkeletalAnimation::WriteAnimationToFile(const char* fileName, const SkeletalAnimation& skeletalAnimation)
{
	BinaryFileWriter fileWriter;

	fileWriter.OpenBinaryFile(fileName);
	WriteToStream(fileWriter, skeletalAnimation);
	fileWriter.CloseBinaryFile();
}



void SkeletalAnimation::ReadAnimationFromFile(const char* fileName, SkeletalAnimation& skeletalAnimation)
{
	BinaryFileReader fileReader;

	fileReader.OpenBinaryFile(fileName);
	ReadFromStream(fileReader, skeletalAnimation);
	fileReader.CloseBinaryFile();
}



void SkeletalAnimation::WriteToStream(const BinaryWriteInterface& streamWriter, const SkeletalAnimation& skeletalAnimation)
{
	streamWriter.Write<uint32_t>(s_FileVersion);

	streamWriter.WriteBinaryString(skeletalAnimation.m_AnimationName);
	streamWriter.Write<uint32_t>(skeletalAnimation.m_NumberOfFrames);
	streamWriter.Write<uint32_t>(skeletalAnimation.m_NumberOfSkeletonJoints);

	streamWriter.Write<float>(skeletalAnimation.m_StartingTime);
	streamWriter.Write<float>(skeletalAnimation.m_DurationInSeconds);
	streamWriter.Write<float>(skeletalAnimation.m_FrameRate);
	streamWriter.Write<float>(skeletalAnimation.m_FrameTime);

	WriteKeyFramesToStream(streamWriter, skeletalAnimation.m_KeyFrames);
	WriteAnimationCurvesToStream(streamWriter, skeletalAnimation.m_AnimationCurves);
}



void SkeletalAnimation::ReadFromStream(const BinaryReadInterface& streamReader, SkeletalAnimation& skeletalAnimation)
{
	uint32_t fileVersion;
	streamReader.Read<uint32_t>(fileVersion);
	if (fileVersion != s_FileVersion)
	{
		ERROR_RECOVERABLE("File version conflict. Different version used.");
		return;
	}

	streamReader.ReadBinaryString(skeletalAnimation.m_AnimationName);
	streamReader.Read<uint32_t>(skeletalAnimation.m_NumberOfFrames);
	streamReader.Read<uint32_t>(skeletalAnimation.m_NumberOfSkeletonJoints);

	streamReader.Read<float>(skeletalAnimation.m_StartingTime);
	streamReader.Read<float>(skeletalAnimation.m_DurationInSeconds);
	streamReader.Read<float>(skeletalAnimation.m_FrameRate);
	streamReader.Read<float>(skeletalAnimation.m_FrameTime);

	uint32_t totalNumberOfKeyFrames = skeletalAnimation.m_NumberOfFrames * skeletalAnimation.m_NumberOfSkeletonJoints;
	skeletalAnimation.m_KeyFrames.resize(totalNumberOfKeyFrames);
	ReadKeyFramesFromStream(streamReader, skeletalAnimation.m_KeyFrames);

	skeletalAnimation.m_AnimationCurves.resize(skeletalAnimation.m_NumberOfSkeletonJoints);
	ReadAnimationCurvesFromStream(streamReader, skeletalAnimation.m_AnimationCurves);
}



void SkeletalAnimation::WriteKeyFramesToStream(const BinaryWriteInterface& streamWriter, const std::vector<Matrix4>& keyFrames)
{
	for (const Matrix4& currentKeyFrame : keyFrames)
	{
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[0]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[1]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[2]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[3]);

		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[4]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[5]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[6]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[7]);

		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[8]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[9]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[10]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[11]);

		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[12]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[13]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[14]);
		streamWriter.Write<float>(currentKeyFrame.m_Matrix4[15]);
	}
}



void SkeletalAnimation::ReadKeyFramesFromStream(const BinaryReadInterface& streamReader, std::vector<Matrix4>& keyFrames)
{
	for (Matrix4& currentKeyFrame : keyFrames)
	{
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[0]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[1]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[2]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[3]);

		streamReader.Read<float>(currentKeyFrame.m_Matrix4[4]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[5]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[6]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[7]);

		streamReader.Read<float>(currentKeyFrame.m_Matrix4[8]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[9]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[10]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[11]);

		streamReader.Read<float>(currentKeyFrame.m_Matrix4[12]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[13]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[14]);
		streamReader.Read<float>(currentKeyFrame.m_Matrix4[15]);
	}
}



void SkeletalAnimation::WriteAnimationCurvesToStream(const BinaryWriteInterface& streamWriter, const std::vector<AnimationCurve>& animationCurves)
{
	for (const AnimationCurve& currentAnimationCurve : animationCurves)
	{
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_ScalingCurve[0]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_ScalingCurve[1]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_ScalingCurve[2]);

		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_RotationCurve[0]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_RotationCurve[1]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_RotationCurve[2]);

		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_TranslationCurve[0]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_TranslationCurve[1]);
		WriteTransformCurveToStream(streamWriter, currentAnimationCurve.m_TranslationCurve[2]);
	}
}



void SkeletalAnimation::ReadAnimationCurvesFromStream(const BinaryReadInterface& streamReader, std::vector<AnimationCurve>& animationCurves)
{
	for (AnimationCurve& currentAnimationCurve : animationCurves)
	{
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_ScalingCurve[0]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_ScalingCurve[1]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_ScalingCurve[2]);

		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_RotationCurve[0]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_RotationCurve[1]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_RotationCurve[2]);

		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_TranslationCurve[0]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_TranslationCurve[1]);
		ReadTransformCurveFromStream(streamReader, currentAnimationCurve.m_TranslationCurve[2]);
	}
}



void SkeletalAnimation::WriteTransformCurveToStream(const BinaryWriteInterface& streamWriter, const TransformCurve& transformCurve)
{
	streamWriter.Write<uint32_t>(transformCurve.m_KeyFrames.size());
	
	for (const KeyFrame& currentKeyFrame : transformCurve.m_KeyFrames)
	{
		streamWriter.Write<float>(currentKeyFrame.frameCurrentTime);
		streamWriter.Write<float>(currentKeyFrame.frameValue);

		switch (currentKeyFrame.currentInterpolationType)
		{
		case CONSTANT_INTERPOLATION:
			streamWriter.WriteBinaryString("Constant");
			break;

		case LINEAR_INTERPOLATION:
			streamWriter.WriteBinaryString("Linear");
			break;

		case CUBIC_INTERPOLATION:
		{
			streamWriter.WriteBinaryString("Cubic");
			streamWriter.Write<float>(currentKeyFrame.positiveSlope);
			streamWriter.Write<float>(currentKeyFrame.negativeSlope);
		}

			break;
		}
	}
}



void SkeletalAnimation::ReadTransformCurveFromStream(const BinaryReadInterface& streamReader, TransformCurve& transformCurve)
{
	uint32_t numberOfKeyFrames;
	streamReader.Read<uint32_t>(numberOfKeyFrames);
	transformCurve.m_KeyFrames.resize(numberOfKeyFrames);
	
	for (KeyFrame& currentKeyFrame : transformCurve.m_KeyFrames)
	{
		streamReader.Read<float>(currentKeyFrame.frameCurrentTime);
		streamReader.Read<float>(currentKeyFrame.frameValue);

		std::string interpolationType;
		streamReader.ReadBinaryString(interpolationType);
		if (interpolationType == "Constant")
		{
			currentKeyFrame.currentInterpolationType = CONSTANT_INTERPOLATION;
		}
		else if (interpolationType == "Linear")
		{
			currentKeyFrame.currentInterpolationType = LINEAR_INTERPOLATION;
		}
		else if (interpolationType == "Cubic")
		{
			currentKeyFrame.currentInterpolationType = CUBIC_INTERPOLATION;
			streamReader.Read<float>(currentKeyFrame.positiveSlope);
			streamReader.Read<float>(currentKeyFrame.negativeSlope);
		}
	}
}



float SkeletalAnimation::GetAnimationTimeForMode(float initialAnimationTime, const SkeletalAnimationMode& animationMode)
{
	float animationTime = initialAnimationTime;

	switch (animationMode)
	{
	case SINGLE_PLAY_MODE:
		animationTime = ClampFloat(initialAnimationTime, 0.0f, GetAnimationDuration());
		break;

	case LOOPING_MODE:
		animationTime = WrapAroundCircularRange(initialAnimationTime, 0.0f, GetAnimationDuration());
		break;

	case PINGPONG_MODE:
		animationTime = OscillateWithinRange(initialAnimationTime, 0.0f, GetAnimationDuration());
		break;

	default:
		break;
	}

	return animationTime;
}