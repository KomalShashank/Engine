#pragma once

#include <string>
#include <vector>

#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"
#include "Engine/IO Utilities/BinaryFileIO.hpp"
#include "Engine/Renderer/SkeletalAnimation/AnimationCurve.hpp"



class MeshSkeletonJoint
{
public:
	MeshSkeletonJoint();

public:
	std::string m_JointName;
	int m_JointParentIndex;
	Matrix4 m_LocalTransform;
	Matrix4 m_ModelToGlobalTransform;
};



class MeshSkeleton
{
public:
	MeshSkeleton();
	~MeshSkeleton();

	void AddJoint(const char* jointName, int parentJointIndex, const Matrix4& initialBoneToModelMatrix);

	void SetJointGlobalTransform(uint32_t jointIndex, const Matrix4& globalTransform);
	void SetJointLocalTransform(uint32_t jointIndex, const Matrix4& localTransform);

	void SetTransformBoneMatrices(Matrix4*& boneTransformMatrices);

	Matrix4 GetLocalTransform(uint32_t jointIndex) const;
	Matrix4 GetGlobalTransform(uint32_t jointIndex) const;

	void AddJointInfo(JointInfo* currentJointInfo);

	std::string GetName(uint32_t jointIndex) const;
	Vector3 GetJointPosition(uint32_t jointIndex) const;
	bool GetBonePositions(uint32_t jointIndex, Vector3& boneStartPosition, Vector3& boneEndPosition) const;

	MeshSkeletonJoint* GetJointByIndex(uint32_t jointIndex) const;
	int FindJointIndex(const std::string& jointName) const;

	uint32_t GetNumberOfJoints() const;
	int GetLastAddedJointIndex() const;
	uint32_t GetNumberOfJointInfos() const;

	JointInfo* GetJointInfoByIndex(uint32_t jointInfoIndex) const;

	void VisualizeMeshSkeleton() const;

	static void WriteSkeletonToFile(const char* fileName, const MeshSkeleton& meshSkeleton);
	static void ReadSkeletonFromFile(const char* fileName, MeshSkeleton& meshSkeleton);

private:
	static void WriteToStream(const BinaryWriteInterface& streamWriter, const MeshSkeleton& meshSkeleton);
	static void ReadFromStream(const BinaryReadInterface& streamReader, MeshSkeleton& meshSkeleton);

	static void WriteJointsToStream(const BinaryWriteInterface& streamWriter, const std::vector<MeshSkeletonJoint*>& skeletonJoints);
	static void ReadJointsFromStream(const BinaryReadInterface& streamReader, std::vector<MeshSkeletonJoint*>& skeletonJoints);

	static void WriteJointInfosToStream(const BinaryWriteInterface& streamWriter, const std::vector<JointInfo*>& jointInfos);
	static void ReadJointInfosFromStream(const BinaryReadInterface& streamReader, std::vector<JointInfo*>& jointInfos);

	void ResizeListOfSkeletonJoints(const uint32_t& numberOfJoints);
	void ResizeListOfJointInfos(const uint32_t& numberOfJointInfos);

	Matrix4 ConvertGlobalToLocalTransform(const Matrix4& globalTransform, uint32_t parentIndex);

private:
	static const uint32_t s_FileVersion = 1;
	std::vector<MeshSkeletonJoint*> m_SkeletonJoints;
	std::vector<JointInfo*> m_JointInfos;
};