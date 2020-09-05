#include "Engine/Renderer/MeshAndMaterial/MeshSkeleton.hpp"
#include "Engine/Renderer/RenderUtilities/BasicRenderer.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



MeshSkeletonJoint::MeshSkeletonJoint() :
m_JointName(""),
m_JointParentIndex(-1),
m_LocalTransform(Matrix4::IdentityMatrix4()),
m_ModelToGlobalTransform(Matrix4::IdentityMatrix4())
{

}



MeshSkeleton::MeshSkeleton()
{

}



MeshSkeleton::~MeshSkeleton()
{
	for (MeshSkeletonJoint* currentJoint : m_SkeletonJoints)
	{
		delete currentJoint;
	}

	for (JointInfo* currentJointInfo : m_JointInfos)
	{
		delete currentJointInfo;
	}
}



void MeshSkeleton::AddJoint(const char* jointName, int parentJointIndex, const Matrix4& initialBoneToModelMatrix)
{
	MeshSkeletonJoint* newJoint = new MeshSkeletonJoint();
	
	Matrix4 initialModelToBoneMatrix;
	initialBoneToModelMatrix.HasInverse(initialModelToBoneMatrix);

	newJoint->m_JointName = std::string(jointName);
	newJoint->m_JointParentIndex = parentJointIndex;
	newJoint->m_LocalTransform = ConvertGlobalToLocalTransform(initialBoneToModelMatrix, parentJointIndex);
	newJoint->m_ModelToGlobalTransform = initialModelToBoneMatrix;

	m_SkeletonJoints.push_back(newJoint);
}



void MeshSkeleton::SetJointGlobalTransform(uint32_t jointIndex, const Matrix4& globalTransform)
{
	uint32_t parentIndex = m_SkeletonJoints[jointIndex]->m_JointParentIndex;
	
	m_SkeletonJoints[jointIndex]->m_LocalTransform = ConvertGlobalToLocalTransform(globalTransform, parentIndex);
}



void MeshSkeleton::SetJointLocalTransform(uint32_t jointIndex, const Matrix4& localTransform)
{
	m_SkeletonJoints[jointIndex]->m_LocalTransform = localTransform;
}



void MeshSkeleton::SetTransformBoneMatrices(Matrix4*& boneTransformMatrices)
{
	uint32_t numberOfBoneMatrices = GetNumberOfJoints();

	for (uint32_t boneMatrixIndex = 0; boneMatrixIndex < numberOfBoneMatrices; ++boneMatrixIndex)
	{
		Matrix4 initialModelToBoneMatrix = m_SkeletonJoints[boneMatrixIndex]->m_ModelToGlobalTransform;
		Matrix4 currentBoneToModelMatrix = GetGlobalTransform(boneMatrixIndex);

		boneTransformMatrices[boneMatrixIndex] = initialModelToBoneMatrix * currentBoneToModelMatrix;
	}
}



Matrix4 MeshSkeleton::GetLocalTransform(uint32_t jointIndex) const
{
	return m_SkeletonJoints[jointIndex]->m_LocalTransform;
}



Matrix4 MeshSkeleton::GetGlobalTransform(uint32_t jointIndex) const
{
	Matrix4 localTransform = GetLocalTransform(jointIndex);
	
	if (jointIndex == 0)
	{
		return localTransform;
	}

	uint32_t parentIndex = m_SkeletonJoints[jointIndex]->m_JointParentIndex;
	Matrix4 parentGlobalTransform = GetGlobalTransform(parentIndex);

	return localTransform * parentGlobalTransform;
}



void MeshSkeleton::AddJointInfo(JointInfo* currentJointInfo)
{
	m_JointInfos.push_back(currentJointInfo);
}



std::string MeshSkeleton::GetName(uint32_t jointIndex) const
{
	return m_SkeletonJoints[jointIndex]->m_JointName;
}



Vector3 MeshSkeleton::GetJointPosition(uint32_t jointIndex) const
{
	Matrix4 boneToModelMatrix = GetGlobalTransform(jointIndex);

	return Vector3(boneToModelMatrix.m_Matrix4[12], boneToModelMatrix.m_Matrix4[13], boneToModelMatrix.m_Matrix4[14]);
}



bool MeshSkeleton::GetBonePositions(uint32_t jointIndex, Vector3& boneStartPosition, Vector3& boneEndPosition) const
{
	if (jointIndex < 0 || jointIndex >= static_cast<int>(m_SkeletonJoints.size()))
	{
		return false;
	}

	int jointParentIndex = m_SkeletonJoints[jointIndex]->m_JointParentIndex;

	if (jointParentIndex == -1)
	{
		return false;
	}

	boneStartPosition = GetJointPosition(jointParentIndex);
	boneEndPosition = GetJointPosition(jointIndex);

	return true;
}



MeshSkeletonJoint* MeshSkeleton::GetJointByIndex(uint32_t jointIndex) const
{
	return m_SkeletonJoints[jointIndex];
}



int MeshSkeleton::FindJointIndex(const std::string& jointName) const
{
	for (size_t jointIndex = 0; jointIndex < m_SkeletonJoints.size(); ++jointIndex)
	{
		if (m_SkeletonJoints[jointIndex]->m_JointName == jointName)
		{
			return static_cast<int>(jointIndex);
		}
	}

	return -1;
}



uint32_t MeshSkeleton::GetNumberOfJoints() const
{
	return m_SkeletonJoints.size();
}



int MeshSkeleton::GetLastAddedJointIndex() const
{
	return GetNumberOfJoints() - 1;
}



uint32_t MeshSkeleton::GetNumberOfJointInfos() const
{
	return m_JointInfos.size();
}



JointInfo* MeshSkeleton::GetJointInfoByIndex(uint32_t jointInfoIndex) const
{
	return m_JointInfos[jointInfoIndex];
}



void MeshSkeleton::VisualizeMeshSkeleton() const
{
	std::vector<Vertex3D> skeletonVertices;
	Vertex3D skeletonVertex;

	for (size_t jointIndex = 0; jointIndex < m_SkeletonJoints.size(); ++jointIndex)
	{
		Vector3 jointPosition = GetJointPosition(jointIndex);
		
		skeletonVertex.m_Color = RGBA::RED;

		skeletonVertex.m_Position = jointPosition - (Vector3::X_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		skeletonVertex.m_Position = jointPosition + (Vector3::X_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		skeletonVertex.m_Position = jointPosition - (Vector3::Y_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		skeletonVertex.m_Position = jointPosition + (Vector3::Y_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		skeletonVertex.m_Position = jointPosition - (Vector3::Z_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		skeletonVertex.m_Position = jointPosition + (Vector3::Z_AXIS * 0.5f);
		skeletonVertices.push_back(skeletonVertex);

		Vector3 boneStartPosition;
		Vector3 boneEndPosition;
		if (GetBonePositions(jointIndex, boneStartPosition, boneEndPosition))
		{
			skeletonVertex.m_Color = RGBA::WHITE;

			skeletonVertex.m_Position = boneStartPosition;
			skeletonVertices.push_back(skeletonVertex);

			skeletonVertex.m_Position = boneEndPosition;
			skeletonVertices.push_back(skeletonVertex);
		}
	}

	g_BasicRenderer->EnableDepthTesting(false);
	g_BasicRenderer->Draw3DVertexArrays(skeletonVertices.data(), skeletonVertices.size(), LINES_PRIMITIVE);
	g_BasicRenderer->EnableDepthTesting(true);
}



void MeshSkeleton::WriteSkeletonToFile(const char* fileName, const MeshSkeleton& meshSkeleton)
{
	BinaryFileWriter fileWriter;

	fileWriter.OpenBinaryFile(fileName);
	WriteToStream(fileWriter, meshSkeleton);
	fileWriter.CloseBinaryFile();
}



void MeshSkeleton::ReadSkeletonFromFile(const char* fileName, MeshSkeleton& meshSkeleton)
{
	BinaryFileReader fileReader;

	fileReader.OpenBinaryFile(fileName);
	ReadFromStream(fileReader, meshSkeleton);
	fileReader.CloseBinaryFile();
}



void MeshSkeleton::WriteToStream(const BinaryWriteInterface& streamWriter, const MeshSkeleton& meshSkeleton)
{
	streamWriter.Write<uint32_t>(s_FileVersion);

	uint32_t numberOfJoints = meshSkeleton.GetNumberOfJoints();
	streamWriter.Write<uint32_t>(numberOfJoints);

	WriteJointsToStream(streamWriter, meshSkeleton.m_SkeletonJoints);

	uint32_t numberOfJointInfos = meshSkeleton.GetNumberOfJointInfos();
	streamWriter.Write<uint32_t>(numberOfJointInfos);

	WriteJointInfosToStream(streamWriter, meshSkeleton.m_JointInfos);
}



void MeshSkeleton::ReadFromStream(const BinaryReadInterface& streamReader, MeshSkeleton& meshSkeleton)
{
	uint32_t fileVersion;
	streamReader.Read<uint32_t>(fileVersion);
	if (fileVersion != s_FileVersion)
	{
		ERROR_RECOVERABLE("File version conflict. Different version used.");
		return;
	}

	uint32_t numberOfJoints;
	streamReader.Read<uint32_t>(numberOfJoints);
	meshSkeleton.ResizeListOfSkeletonJoints(numberOfJoints);

	ReadJointsFromStream(streamReader, meshSkeleton.m_SkeletonJoints);

	uint32_t numberOfJointInfos;
	streamReader.Read<uint32_t>(numberOfJointInfos);
	meshSkeleton.ResizeListOfJointInfos(numberOfJointInfos);

	ReadJointInfosFromStream(streamReader, meshSkeleton.m_JointInfos);
}



void MeshSkeleton::WriteJointsToStream(const BinaryWriteInterface& streamWriter, const std::vector<MeshSkeletonJoint*>& skeletonJoints)
{
	for (const MeshSkeletonJoint* currentJoint : skeletonJoints)
	{
		streamWriter.WriteBinaryString(currentJoint->m_JointName);
		streamWriter.Write<int32_t>(currentJoint->m_JointParentIndex);
		streamWriter.WriteMatrix4(currentJoint->m_LocalTransform);
		streamWriter.WriteMatrix4(currentJoint->m_ModelToGlobalTransform);
	}
}



void MeshSkeleton::ReadJointsFromStream(const BinaryReadInterface& streamReader, std::vector<MeshSkeletonJoint*>& skeletonJoints)
{
	for (MeshSkeletonJoint* currentJoint : skeletonJoints)
	{
		streamReader.ReadBinaryString(currentJoint->m_JointName);
		streamReader.Read<int32_t>(currentJoint->m_JointParentIndex);
		streamReader.ReadMatrix4(currentJoint->m_LocalTransform);
		streamReader.ReadMatrix4(currentJoint->m_ModelToGlobalTransform);
	}
}



void MeshSkeleton::WriteJointInfosToStream(const BinaryWriteInterface& streamWriter, const std::vector<JointInfo*>& jointInfos)
{
	for (const JointInfo* currentJointInfo : jointInfos)
	{
		streamWriter.WriteVector3(currentJointInfo->scalePivot);
		streamWriter.WriteVector3(currentJointInfo->scaleOffset);
		streamWriter.WriteVector3(currentJointInfo->rotationPivot);
		streamWriter.WriteVector3(currentJointInfo->rotationOffset);

		streamWriter.WriteMatrix4(currentJointInfo->preRotationMatrix);
		streamWriter.WriteMatrix4(currentJointInfo->postRotationMatrix);
	}
}



void MeshSkeleton::ReadJointInfosFromStream(const BinaryReadInterface& streamReader, std::vector<JointInfo*>& jointInfos)
{
	for (JointInfo* currentJointInfo : jointInfos)
	{
		streamReader.ReadVector3(currentJointInfo->scalePivot);
		streamReader.ReadVector3(currentJointInfo->scaleOffset);
		streamReader.ReadVector3(currentJointInfo->rotationPivot);
		streamReader.ReadVector3(currentJointInfo->rotationOffset);

		streamReader.ReadMatrix4(currentJointInfo->preRotationMatrix);
		streamReader.ReadMatrix4(currentJointInfo->postRotationMatrix);
	}
}



void MeshSkeleton::ResizeListOfSkeletonJoints(const uint32_t& numberOfJoints)
{
	m_SkeletonJoints.resize(numberOfJoints);

	for (uint32_t jointIndex = 0; jointIndex < numberOfJoints; ++jointIndex)
	{
		m_SkeletonJoints[jointIndex] = new MeshSkeletonJoint();
	}
}



void MeshSkeleton::ResizeListOfJointInfos(const uint32_t& numberOfJointInfos)
{
	m_JointInfos.resize(numberOfJointInfos);

	for (uint32_t jointInfoIndex = 0; jointInfoIndex < numberOfJointInfos; ++jointInfoIndex)
	{
		m_JointInfos[jointInfoIndex] = new JointInfo();
	}
}



Matrix4 MeshSkeleton::ConvertGlobalToLocalTransform(const Matrix4& globalTransform, uint32_t parentIndex)
{
	if (parentIndex == -1)
	{
		return globalTransform;
	}

	Matrix4 parentGlobalTransform = GetGlobalTransform(parentIndex);

	Matrix4 parentGlobalTransformInverse;
	parentGlobalTransform.HasInverse(parentGlobalTransformInverse);

	return globalTransform * parentGlobalTransformInverse;
}