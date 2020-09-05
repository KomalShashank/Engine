#pragma once

#include <string>
#include <vector>

#include "Engine/Math/MatrixMath/Matrix4.hpp"
#include "Engine/Renderer/MeshAndMaterial/MeshSkeleton.hpp"
#include "Engine/IO Utilities/BinaryFileIO.hpp"
#include "Engine/Renderer/SkeletalAnimation/AnimationCurve.hpp"



enum SkeletalAnimationMode
{
	SINGLE_PLAY_MODE,
	LOOPING_MODE,
	PINGPONG_MODE
};



class SkeletalAnimation
{
public:
	SkeletalAnimation();
	SkeletalAnimation(const std::string& animationName, float startingTime, float durationInSeconds, float frameRate, MeshSkeleton* skeleton);

	Matrix4* GetKeyFramesForJointIndex(uint32_t jointIndex);
	void GetFrameIndicesAndBlendFactor(uint32_t& startingFrameIndex, uint32_t& endingFrameIndex, float& blendFactor, float animationTime);
	
	uint32_t GetNumberOfFrames() const;
	float GetStartingTime() const;
	float GetAnimationDuration() const;
	
	float GetFrameRate() const;
	float GetFrameTime() const;

	void InsertAnimationCurve(const AnimationCurve& animationCurve);

	void ApplyKeyFrameAnimation(MeshSkeleton* skeleton, float currentAnimationTime, const SkeletalAnimationMode& animationMode);
	void ApplyCurveAnimation(MeshSkeleton* skeleton, float currentAnimationTime, const SkeletalAnimationMode& animationMode);

	static void WriteAnimationToFile(const char* fileName, const SkeletalAnimation& skeletalAnimation);
	static void ReadAnimationFromFile(const char* fileName, SkeletalAnimation& skeletalAnimation);

private:
	static void WriteToStream(const BinaryWriteInterface& streamWriter, const SkeletalAnimation& skeletalAnimation);
	static void ReadFromStream(const BinaryReadInterface& streamReader, SkeletalAnimation& skeletalAnimation);

	static void WriteKeyFramesToStream(const BinaryWriteInterface& streamWriter, const std::vector<Matrix4>& keyFrames);
	static void ReadKeyFramesFromStream(const BinaryReadInterface& streamReader, std::vector<Matrix4>& keyFrames);

	static void WriteAnimationCurvesToStream(const BinaryWriteInterface& streamWriter, const std::vector<AnimationCurve>& animationCurves);
	static void ReadAnimationCurvesFromStream(const BinaryReadInterface& streamReader, std::vector<AnimationCurve>& animationCurves);

	static void WriteTransformCurveToStream(const BinaryWriteInterface& streamWriter, const TransformCurve& transformCurve);
	static void ReadTransformCurveFromStream(const BinaryReadInterface& streamReader, TransformCurve& transformCurve);

	float GetAnimationTimeForMode(float initialAnimationTime, const SkeletalAnimationMode& animationMode);

private:
	static const uint32_t s_FileVersion = 1;

	std::string m_AnimationName;
	uint32_t m_NumberOfFrames;
	uint32_t m_NumberOfSkeletonJoints;

	float m_StartingTime;
	float m_DurationInSeconds;
	float m_FrameRate;
	float m_FrameTime;

	std::vector<Matrix4> m_KeyFrames;
	std::vector<AnimationCurve> m_AnimationCurves;
};