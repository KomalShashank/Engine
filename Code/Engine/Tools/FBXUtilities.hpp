#pragma once

#ifndef __TOOLS_FBX__
#define __TOOLS_FBX__

#include <string>
#include <vector>

#include "Engine/Renderer/MeshAndMaterial/MeshBuilder.hpp"
#include "Engine/Renderer/MeshAndMaterial/MeshSkeleton.hpp"
#include "Engine/Renderer/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"

struct SceneImport
{
public:
	std::vector<MeshBuilder*> m_Meshes;
	std::vector<MeshSkeleton*> m_Skeletons;
	std::vector<SkeletalAnimation*> m_Animations;
};

void FBXListScene(const std::string& fileName);
SceneImport* FBXLoadSceneFromFile(const std::string& fileName, const Matrix4& localBasis, const Matrix4& transformMatrix = Matrix4::IdentityMatrix4(), bool isRightHanded = true);

#endif // __TOOLS_FBX__