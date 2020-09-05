#include "Engine/Tools/FBXUtilities.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/DeveloperConsole/DeveloperConsole.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"
#include "Engine/Math/MatrixMath/MatrixStack.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/VectorMath/4D/IntVector4.hpp"
#include "Engine/Renderer/SkeletalAnimation/AnimationCurve.hpp"



#ifdef TOOLS_BUILD

#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk-md.lib")

class BoneWeights
{
public:
	Vector4 m_Weights;
	IntVector4 m_Indices;

public:
	BoneWeights() :
	m_Weights(Vector4::ZERO),
	m_Indices(IntVector4::ZERO)
	{

	}
};

static std::string GetAttributeTypeName(FbxNodeAttribute::EType currentAttributeType)
{
	switch (currentAttributeType)
	{
	case FbxNodeAttribute::eUnknown:
		return "Unknown";
	
	case FbxNodeAttribute::eNull:
		return "Null";
	
	case FbxNodeAttribute::eMarker:
		return "Marker";
	
	case FbxNodeAttribute::eSkeleton:
		return "Skeleton";
	
	case FbxNodeAttribute::eMesh:
		return "Mesh";
	
	case FbxNodeAttribute::eNurbs:
		return "Nurbs";
	
	case FbxNodeAttribute::ePatch:
		return "Patch";
	
	case FbxNodeAttribute::eCamera:
		return "Camera";
	
	case FbxNodeAttribute::eCameraStereo:
		return "Stereo";
	
	case FbxNodeAttribute::eCameraSwitcher:
		return "Camera Switcher";
	
	case FbxNodeAttribute::eLight:
		return "Light";
	
	case FbxNodeAttribute::eOpticalReference:
		return "Optical Reference";
	
	case FbxNodeAttribute::eOpticalMarker:
		return "Marker";
	
	case FbxNodeAttribute::eNurbsCurve:
		return "Nurbs Curve";
	
	case FbxNodeAttribute::eTrimNurbsSurface:
		return "Trim Nurbs Surface";
	
	case FbxNodeAttribute::eBoundary:
		return "Boundary";
	
	case FbxNodeAttribute::eNurbsSurface:
		return "Nurbs Surface";
	
	case FbxNodeAttribute::eShape:
		return "Shape";
	
	case FbxNodeAttribute::eLODGroup:
		return "LODGroup";
	
	case FbxNodeAttribute::eSubDiv:
		return "SubDiv";
	
	default:
		return "Undefined"; }
}



static void PrintAttribute(FbxNodeAttribute* currentNodeAttribute, int treeDepth)
{
	if (currentNodeAttribute == nullptr)
	{
		return;
	}

	FbxNodeAttribute::EType attributeType = currentNodeAttribute->GetAttributeType();

	const std::string attributeTypeName = GetAttributeTypeName(attributeType);
	const std::string attributeName = currentNodeAttribute->GetName();

	ConsoleLine printAttributeMessage = ConsoleLine(Stringf("%*s- Type = '%s', Name = '%s'", treeDepth * 4, " ", attributeTypeName.c_str(), attributeName.c_str()), RGBA::WHITE);
	DeveloperConsole::AddNewConsoleLine(printAttributeMessage);
}



static void PrintNode(FbxNode* currentNode, int treeDepth)
{
	ConsoleLine printNodeMessage = ConsoleLine(Stringf("%*sNode[%s]\n", treeDepth * 4, " ", currentNode->GetName()), RGBA::GREEN);
	DeveloperConsole::AddNewConsoleLine(printNodeMessage);

	for (int index = 0; index < currentNode->GetNodeAttributeCount(); ++index)
	{
		PrintAttribute(currentNode->GetNodeAttributeByIndex(index), treeDepth);
	}

	for (int index = 0; index < currentNode->GetChildCount(); ++index)
	{
		PrintNode(currentNode->GetChild(index), treeDepth + 1);
	}
}



void FBXListScene(const std::string& fileName)
{
	FbxManager* fbxManager = FbxManager::Create();
	if (fbxManager == nullptr)
	{
		ConsoleLine loadMessage = ConsoleLine("Could not create FBX Manager.", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(loadMessage);

		return;
	}

	FbxIOSettings* IOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	IOSettings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, false);
	fbxManager->SetIOSettings(IOSettings);

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool initializeResult = fbxImporter->Initialize(fileName.c_str(), -1, fbxManager->GetIOSettings());

	if (!initializeResult)
	{
		ConsoleLine initializeMessage = ConsoleLine(Stringf("Could not import scene: %s", fileName.c_str()), RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(initializeMessage);
	}
	else
	{
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
		bool importResult = fbxImporter->Import(fbxScene);

		if (importResult)
		{
			FbxNode* rootNode = fbxScene->GetRootNode();
			PrintNode(rootNode, 0);
		}

		FBX_SAFE_DESTROY(fbxScene);
	}

	FBX_SAFE_DESTROY(fbxImporter);
	FBX_SAFE_DESTROY(IOSettings);
	FBX_SAFE_DESTROY(fbxManager);
}



static Vector3 ConvertFBXVector4ToVector3(const FbxVector4& fbxPosition)
{
	return Vector3(static_cast<float>(fbxPosition.mData[0]), static_cast<float>(fbxPosition.mData[1]), static_cast<float>(fbxPosition.mData[2]));
}



static Vector4 ConvertFBXDouble4ToVector4(const FbxDouble4& fbxDouble4)
{
	return Vector4(static_cast<float>(fbxDouble4.mData[0]), static_cast<float>(fbxDouble4.mData[1]), static_cast<float>(fbxDouble4.mData[2]), static_cast<float>(fbxDouble4.mData[3]));
}



static RGBA ConvertFBXColorToRGBA(const FbxColor& fbxColor)
{
	Vector4 colorAsFloats(static_cast<float>(fbxColor.mRed), static_cast<float>(fbxColor.mGreen), static_cast<float>(fbxColor.mBlue), static_cast<float>(fbxColor.mAlpha));

	colorAsFloats.X = RangeMap(colorAsFloats.X, 0.0f, 1.0f, 0.0f, 255.0f);
	colorAsFloats.Y = RangeMap(colorAsFloats.Y, 0.0f, 1.0f, 0.0f, 255.0f);
	colorAsFloats.Z = RangeMap(colorAsFloats.Z, 0.0f, 1.0f, 0.0f, 255.0f);
	colorAsFloats.T = RangeMap(colorAsFloats.T, 0.0f, 1.0f, 0.0f, 255.0f);

	RGBA vertexColor(static_cast<unsigned char>(colorAsFloats.X), static_cast<unsigned char>(colorAsFloats.Y), static_cast<unsigned char>(colorAsFloats.Z), static_cast<unsigned char>(colorAsFloats.T));

	return vertexColor;
}



static Matrix4 ConvertToLocalMatrix(const FbxMatrix& sceneMatrix)
{
	Matrix4 fbxSceneBasis = Matrix4::IdentityMatrix4();
	
	fbxSceneBasis.m_Matrix4[0] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[0]).X;
	fbxSceneBasis.m_Matrix4[1] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[0]).Y;
	fbxSceneBasis.m_Matrix4[2] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[0]).Z;
	fbxSceneBasis.m_Matrix4[3] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[0]).T;

	fbxSceneBasis.m_Matrix4[4] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[1]).X;
	fbxSceneBasis.m_Matrix4[5] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[1]).Y;
	fbxSceneBasis.m_Matrix4[6] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[1]).Z;
	fbxSceneBasis.m_Matrix4[7] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[1]).T;

	fbxSceneBasis.m_Matrix4[8] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[2]).X;
	fbxSceneBasis.m_Matrix4[9] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[2]).Y;
	fbxSceneBasis.m_Matrix4[10] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[2]).Z;
	fbxSceneBasis.m_Matrix4[11] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[2]).T;

	fbxSceneBasis.m_Matrix4[12] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[3]).X;
	fbxSceneBasis.m_Matrix4[13] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[3]).Y;
	fbxSceneBasis.m_Matrix4[14] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[3]).Z;
	fbxSceneBasis.m_Matrix4[15] = ConvertFBXDouble4ToVector4(sceneMatrix.mData[3]).T;

	return fbxSceneBasis;
}



static bool GetPosition(Vector3& vertexPosition, const Matrix4& transformMatrix, FbxMesh* fbxMesh, int polyIndex, int vertexIndex)
{
	FbxVector4 fbxPosition;
	int controlIndex = fbxMesh->GetPolygonVertex(polyIndex, vertexIndex);
	fbxPosition = fbxMesh->GetControlPointAt(controlIndex);
	vertexPosition = transformMatrix.TransformVector3(ConvertFBXVector4ToVector3(fbxPosition), 1.0f);

	return true;
}



template <typename TElement, typename TObject>
static bool GetObjectFromElement(FbxMesh* fbxMesh, int polyIndex, int vertexIndex, TElement* element, TObject& object)
{
	if (element == nullptr)
	{
		return false;
	}

	int elementIndex = 0;

	switch (element->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		elementIndex = fbxMesh->GetPolygonVertex(polyIndex, vertexIndex);
	}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		elementIndex = fbxMesh->GetPolygonVertexIndex(polyIndex) + vertexIndex;
	}
		break;

	default:
		break;
	}

	switch (element->GetReferenceMode())
	{
	case FbxGeometryElement::eDirect:
	{
		if (elementIndex < element->GetDirectArray().GetCount())
		{
			object = element->GetDirectArray().GetAt(elementIndex);
			return true;
		}
	}
		break;

	case FbxGeometryElement::eIndexToDirect:
	{
		if (elementIndex < element->GetIndexArray().GetCount())
		{
			int index = element->GetIndexArray().GetAt(elementIndex);
			object = element->GetDirectArray().GetAt(index);
			return true;
		}
	}
		break;

	default:
		break;
	}

	return false;
}



static bool GetColor(RGBA& vertexColor, FbxMesh* fbxMesh, int polyIndex, int vertexIndex)
{
	FbxColor fbxColor;
	FbxGeometryElementVertexColor* geometryElement = fbxMesh->GetElementVertexColor(0);
	if (GetObjectFromElement(fbxMesh, polyIndex, vertexIndex, geometryElement, fbxColor))
	{
		vertexColor = ConvertFBXColorToRGBA(fbxColor);
		return true;
	}

	return false;
}



static bool GetTextureCoordinates(Vector2& vertexTextureCoordinates, FbxMesh* fbxMesh, int polyIndex, int vertexIndex, int textureCoordinatesIndex)
{
	FbxVector2 fbxTextureCoordinates;
	FbxGeometryElementUV* geometryElement = fbxMesh->GetElementUV(textureCoordinatesIndex);
	if (GetObjectFromElement(fbxMesh, polyIndex, vertexIndex, geometryElement, fbxTextureCoordinates))
	{
		vertexTextureCoordinates = Vector2(static_cast<float>(fbxTextureCoordinates.mData[0]), static_cast<float>(fbxTextureCoordinates.mData[1]));
		return true;
	}

	return false;
}



static bool GetTangent(Vector3& vertexTangent, const Matrix4& transformMatrix, FbxMesh* fbxMesh, int polyIndex, int vertexIndex)
{
	FbxVector4 fbxTangent;
	FbxGeometryElementTangent* geometryElement = fbxMesh->GetElementTangent(0);
	if (GetObjectFromElement(fbxMesh, polyIndex, vertexIndex, geometryElement, fbxTangent))
	{
		vertexTangent = transformMatrix.TransformVector3(ConvertFBXVector4ToVector3(fbxTangent), 0.0f);
		return true;
	}

	return false;
}



static bool GetBitangent(Vector3& vertexBitangent, const Matrix4& transformMatrix, FbxMesh* fbxMesh, int polyIndex, int vertexIndex)
{
	FbxVector4 fbxBitangent;
	FbxGeometryElementBinormal* geometryElement = fbxMesh->GetElementBinormal(0);
	if (GetObjectFromElement(fbxMesh, polyIndex, vertexIndex, geometryElement, fbxBitangent))
	{
		vertexBitangent = transformMatrix.TransformVector3(ConvertFBXVector4ToVector3(fbxBitangent), 0.0f);
		return true;
	}

	return false;
}



static bool GetNormal(Vector3& vertexNormal, const Matrix4& transformMatrix, FbxMesh* fbxMesh, int polyIndex, int vertexIndex)
{
	FbxVector4 fbxNormal;
	FbxGeometryElementNormal* geometryElement = fbxMesh->GetElementNormal(0);
	if (GetObjectFromElement(fbxMesh, polyIndex, vertexIndex, geometryElement, fbxNormal))
	{
		vertexNormal = transformMatrix.TransformVector3(ConvertFBXVector4ToVector3(fbxNormal), 0.0f);
		return true;
	}

	return false;
}



static void ImportVertex(MeshBuilder* meshBuilder, const Matrix4& transformMatrix, FbxMesh* fbxMesh, int polyIndex, int vertexIndex, std::vector<BoneWeights>& allBoneWeights)
{
	meshBuilder->ResetVertex();
	
	Vector3 vertexPosition;
	if (GetPosition(vertexPosition, transformMatrix, fbxMesh, polyIndex, vertexIndex))
	{
		meshBuilder->SetVertexPosition(vertexPosition);
	}

	RGBA vertexColor;
	if (GetColor(vertexColor, fbxMesh, polyIndex, vertexIndex))
	{
		meshBuilder->SetVertexColor(vertexColor);
	}

	Vector2 vertexTextureCoordinates;
	if (GetTextureCoordinates(vertexTextureCoordinates, fbxMesh, polyIndex, vertexIndex, 0))
	{
		meshBuilder->SetVertexTextureCoordinates(vertexTextureCoordinates);
	}

	Vector3 vertexTangent;
	if (GetTangent(vertexTangent, transformMatrix, fbxMesh, polyIndex, vertexIndex))
	{
		meshBuilder->SetVertexTangent(vertexTangent.GetNormalizedVector3());
	}

	Vector3 vertexBitangent;
	if (GetBitangent(vertexBitangent, transformMatrix, fbxMesh, polyIndex, vertexIndex))
	{
		meshBuilder->SetVertexBitangent(vertexBitangent.GetNormalizedVector3());
	}

	Vector3 vertexNormal;
	if (GetNormal(vertexNormal, transformMatrix, fbxMesh, polyIndex, vertexIndex))
	{
		meshBuilder->SetVertexNormal(vertexNormal.GetNormalizedVector3());
	}

	size_t controlIndex = fbxMesh->GetPolygonVertex(polyIndex, vertexIndex);
	if (controlIndex < allBoneWeights.size())
	{
		meshBuilder->SetBoneWeights(allBoneWeights[controlIndex].m_Weights, allBoneWeights[controlIndex].m_Indices);
	}

	meshBuilder->AddVertexToAllVertices();
}



static Matrix4 GetGeometricTransform(FbxNode* fbxNode)
{
	Matrix4 geometrixMatrix = Matrix4::IdentityMatrix4();

	if ((fbxNode != nullptr) && (fbxNode->GetNodeAttribute() != nullptr))
	{
		const FbxVector4 geometricTranslation = fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 geometricRotation = fbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 geometricScaling = fbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxMatrix trsMatrix;
		trsMatrix.SetTRS(geometricTranslation, geometricRotation, geometricScaling);
		geometrixMatrix = ConvertToLocalMatrix(trsMatrix);
	}

	return geometrixMatrix;
}



static std::string GetMaterialID(FbxMesh* fbxMesh)
{
	FbxNode* fbxNode = fbxMesh->GetNode();
	if (fbxNode != nullptr)
	{
		FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(0);
		if (fbxSurfaceMaterial != nullptr)
		{
			const char* fbxMaterialName = fbxSurfaceMaterial->GetName();
			return std::string(fbxMaterialName);
		}
	}

	return std::string("");
}



static int GetJointIndexForNode(const FbxNode* fbxNode, const std::vector<FbxNode*>& jointNodes)
{
	int jointIndex = -1;

	for (size_t currentNodeIndex = 0; currentNodeIndex < jointNodes.size(); ++currentNodeIndex)
	{
		if (jointNodes[currentNodeIndex] == fbxNode)
		{
			jointIndex = currentNodeIndex;
			break;
		}
	}

	return jointIndex;
}



static void AddHighestWeightToBoneWeights(BoneWeights* currentBoneWeights, int jointIndex, float currentWeight)
{
	float lowestWeight = GetMinimumOfFloatArray((float*)(&currentBoneWeights->m_Weights), 4);

	if (lowestWeight == currentBoneWeights->m_Weights.X)
	{
		currentBoneWeights->m_Weights.X = currentWeight;
		currentBoneWeights->m_Indices.X = jointIndex;
	}
	else if (lowestWeight == currentBoneWeights->m_Weights.Y)
	{
		currentBoneWeights->m_Weights.Y = currentWeight;
		currentBoneWeights->m_Indices.Y = jointIndex;
	}
	else if (lowestWeight == currentBoneWeights->m_Weights.Z)
	{
		currentBoneWeights->m_Weights.Z = currentWeight;
		currentBoneWeights->m_Indices.Z = jointIndex;
	}
	else if (lowestWeight == currentBoneWeights->m_Weights.T)
	{
		currentBoneWeights->m_Weights.T = currentWeight;
		currentBoneWeights->m_Indices.T = jointIndex;
	}
}



static void GetBoneWeights(const FbxMesh* fbxMesh, std::vector<BoneWeights>& allBoneWeights, const std::vector<FbxNode*>& jointNodes)
{
	int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	ASSERT_OR_DIE(numberOfDeformers == 1, "Only one deformer is supported.");
	
	for (int deformerIndex = 0; deformerIndex < numberOfDeformers; ++deformerIndex)
	{
		FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		if (fbxSkin == nullptr)
		{
			continue;
		}

		int numberOfClusters = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < numberOfClusters; ++clusterIndex)
		{
			FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);
			const FbxNode* linkNode = fbxCluster->GetLink();

			if (linkNode == nullptr)
			{
				continue;
			}

			int jointIndex = GetJointIndexForNode(linkNode, jointNodes);
			if (jointIndex == -1)
			{
				continue;
			}

			int* controlPointIndices = fbxCluster->GetControlPointIndices();
			int numberOfIndices = fbxCluster->GetControlPointIndicesCount();
			double* controlPointWeights = fbxCluster->GetControlPointWeights();

			for (int controlIndex = 0; controlIndex < numberOfIndices; ++controlIndex)
			{
				int currentIndex = controlPointIndices[controlIndex];
				double currentWeight = controlPointWeights[controlIndex];

				BoneWeights* currentBoneWeights = &allBoneWeights[currentIndex];
				AddHighestWeightToBoneWeights(currentBoneWeights, jointIndex, static_cast<float>(currentWeight));
			}
		}
	}
}



static bool BoneWeightsExistFor(FbxMesh* fbxMesh)
{
	int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	return (numberOfDeformers > 0);
}



static void ImportMesh(SceneImport* fbxSceneImport, FbxMesh* fbxMesh, MatrixStack& matrixStack, const std::vector<FbxNode*>& jointNodes)
{
	MeshBuilder* meshBuilder = new MeshBuilder();
	ASSERT_OR_DIE(fbxMesh->IsTriangleMesh(), "FBX file has non-triangle Mesh.");

	//Import Mesh
	meshBuilder->BeginMeshBuilding();

	Matrix4 geometricTransform = GetGeometricTransform(fbxMesh->GetNode());
	matrixStack.PushToMatrixStack(geometricTransform);

	int numberOfControlPoints = fbxMesh->GetControlPointsCount();
	
	std::vector<BoneWeights> allBoneWeights;
	allBoneWeights.resize(numberOfControlPoints);
	
	if (BoneWeightsExistFor(fbxMesh))
	{
		GetBoneWeights(fbxMesh, allBoneWeights, jointNodes);
	}
	else
	{
// 		FbxNode* fbxNode = fbxMesh->GetNode();
// 		int jointIndex = GetJointIndexForNode(fbxNode, jointNodes);
// 
// 		if (jointIndex == -1)
// 		{
// 			while (jointIndex == -1)
// 			{
// 				fbxNode = fbxNode->GetParent();
// 				jointIndex = GetJointIndexForNode(fbxNode, jointNodes);
// 			}
// 		}
// 
// 		int numberOfChildNodes = fbxNode->GetChildCount(true);
// 
// 		for (int childIndex = 0; childIndex < numberOfChildNodes; ++childIndex)
// 		{
// 			FbxNode* childNode = fbxNode->GetChild(childIndex);
// 		}
	}

	Matrix4 transformMatrix = matrixStack.GetTop();

	int polyCount = fbxMesh->GetPolygonCount();
	for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
	{
		int vertexCount = fbxMesh->GetPolygonSize(polyIndex);
		ASSERT_OR_DIE(vertexCount == 3, "Not a triangle polygon.");
		for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{
			ImportVertex(meshBuilder, transformMatrix, fbxMesh, polyIndex, vertexIndex, allBoneWeights);
		}
	}

	std::string materialID = GetMaterialID(fbxMesh);
	meshBuilder->EndMeshBuilding(materialID);

	matrixStack.PopFromMatrixStack();

	fbxSceneImport->m_Meshes.push_back(meshBuilder);
}



static void ImportNodeAttribute(SceneImport* fbxSceneImport, FbxNodeAttribute* fbxNodeAttribute, MatrixStack& matrixStack, const std::vector<FbxNode*>& jointNodes)
{
	if (fbxNodeAttribute == nullptr)
	{
		return;
	}

	switch (fbxNodeAttribute->GetAttributeType())
	{
	case FbxNodeAttribute::eMesh:
		ImportMesh(fbxSceneImport, reinterpret_cast<FbxMesh*>(fbxNodeAttribute), matrixStack, jointNodes);
		break;

	default:
		break;
	}
}



static void GetJointInfoFromNode(FbxNode* fbxNode, JointInfo* jointInfo)
{
	FbxNode::EPivotSet jointPivot = FbxNode::eSourcePivot;
	FbxEuler::EOrder rotationOrder;

	fbxNode->GetRotationOrder(jointPivot, rotationOrder);

	FbxAMatrix fbxPreRotation;
	fbxPreRotation.SetR(fbxNode->GetPreRotation(jointPivot), rotationOrder);

	FbxAMatrix fbxPostRotation;
	fbxPostRotation.SetR(fbxNode->GetPostRotation(jointPivot), rotationOrder);

	jointInfo->scalePivot = ConvertFBXVector4ToVector3(fbxNode->GetScalingPivot(jointPivot));
	jointInfo->rotationPivot = ConvertFBXVector4ToVector3(fbxNode->GetRotationPivot(jointPivot));
	jointInfo->scaleOffset = ConvertFBXVector4ToVector3(fbxNode->GetScalingOffset(jointPivot));
	jointInfo->rotationOffset = ConvertFBXVector4ToVector3(fbxNode->GetRotationOffset(jointPivot));

	jointInfo->preRotationMatrix = ConvertToLocalMatrix(fbxPreRotation);
	jointInfo->postRotationMatrix = ConvertToLocalMatrix(fbxPostRotation);
}



static Matrix4 GetLocalNodeTransform(FbxNode* fbxNode)
{
// 	FbxMatrix fbxLocalTransform = fbxNode->EvaluateLocalTransform();
// 	Matrix4 nodeLocalTransform = ConvertToLocalMatrix(fbxLocalTransform);
// 
// 	return nodeLocalTransform;

	Matrix4 geometricTransform = GetGeometricTransform(fbxNode);

	FbxNode::EPivotSet jointPivot = FbxNode::eSourcePivot;
	FbxEuler::EOrder rotationOrder;
	fbxNode->GetRotationOrder(jointPivot, rotationOrder);

	JointInfo jointInfo;
	GetJointInfoFromNode(fbxNode, &jointInfo);

	FbxAMatrix fbxScale;
	FbxDouble3 fbxLocalScale = fbxNode->LclScaling;
	FbxVector4 fbxLocalScaleVector = FbxVector4(fbxLocalScale.mData[0], fbxLocalScale.mData[1], fbxLocalScale.mData[2], 1.0f);
	fbxScale.SetS(fbxLocalScaleVector);

	FbxAMatrix fbxRotation;
	FbxDouble3 fbxLocalRotation = fbxNode->LclRotation;
	FbxVector4 fbxLocalRotationVector = FbxVector4(fbxLocalRotation.mData[0], fbxLocalRotation.mData[1], fbxLocalRotation.mData[2], 1.0f);
	fbxRotation.SetR(fbxLocalRotationVector, rotationOrder);

	FbxAMatrix fbxTranslation;
	FbxDouble3 fbxLocalTranslation = fbxNode->LclTranslation;
	FbxVector4 fbxLocalTranslationVector = FbxVector4(fbxLocalTranslation.mData[0], fbxLocalTranslation.mData[1], fbxLocalTranslation.mData[2], 1.0f);
	fbxTranslation.SetT(fbxLocalTranslationVector);

	Matrix4 localScale = ConvertToLocalMatrix(fbxScale);
	Matrix4 localRotation = ConvertToLocalMatrix(fbxRotation);
	Matrix4 localTranslation = ConvertToLocalMatrix(fbxTranslation);

	Matrix4 nodeLocalTransform = jointInfo.CalculateLocalTransform(localScale, localRotation, localTranslation);

	return nodeLocalTransform;
}



static void ImportMeshNode(SceneImport* fbxSceneImport, FbxNode* fbxNode, MatrixStack& matrixStack, const std::vector<FbxNode*>& jointNodes)
{
	if (fbxNode == nullptr)
	{
		return;
	}

	Matrix4 nodeLocalTransform = GetLocalNodeTransform(fbxNode);
	matrixStack.PushToMatrixStack(nodeLocalTransform);

	int attributeCount = fbxNode->GetNodeAttributeCount();
	for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
	{
		ImportNodeAttribute(fbxSceneImport, fbxNode->GetNodeAttributeByIndex(attributeIndex), matrixStack, jointNodes);
	}

	int childCount = fbxNode->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; ++childIndex)
	{
		ImportMeshNode(fbxSceneImport, fbxNode->GetChild(childIndex), matrixStack, jointNodes);
	}

	matrixStack.PopFromMatrixStack();
}



static MeshSkeleton* ImportSkeletonJoint(SceneImport* fbxSceneImport, MatrixStack& matrixStack, MeshSkeleton* currentSkeleton, int parentJointIndex, FbxSkeleton* fbxSkeleton, std::vector<FbxNode*>& jointNodes)
{
	MeshSkeleton* resultantSkeleton = nullptr;

	if (fbxSkeleton->IsSkeletonRoot())
	{
		resultantSkeleton = new MeshSkeleton();
		fbxSceneImport->m_Skeletons.push_back(resultantSkeleton);
	}
	else
	{
		resultantSkeleton = currentSkeleton;
		ASSERT_OR_DIE(resultantSkeleton != nullptr, "Resultant Mesh Skeleton does not exist.");
	}

	Matrix4 geometricTransform = GetGeometricTransform(fbxSkeleton->GetNode());
	matrixStack.PushToMatrixStack(geometricTransform);

	Matrix4 modelMatrix = matrixStack.GetTop();

	resultantSkeleton->AddJoint(fbxSkeleton->GetNode()->GetName(), parentJointIndex, modelMatrix);
	jointNodes.push_back(fbxSkeleton->GetNode());

	JointInfo* jointInfo = new JointInfo();
	GetJointInfoFromNode(fbxSkeleton->GetNode(), jointInfo);
	resultantSkeleton->AddJointInfo(jointInfo);

	matrixStack.PopFromMatrixStack();

	return resultantSkeleton;
}



static void ImportSkeletonNode(SceneImport* fbxSceneImport, FbxNode* fbxNode, MatrixStack& matrixStack, MeshSkeleton* currentSkeleton, std::vector<FbxNode*>& jointNodes, int parentJointIndex)
{
	if (fbxNode == nullptr)
	{
		return;
	}

	Matrix4 transformMatrix = GetLocalNodeTransform(fbxNode);
	matrixStack.PushToMatrixStack(transformMatrix);

	int attributeCount = fbxNode->GetNodeAttributeCount();
	for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
	{
		FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttributeByIndex(attributeIndex);
		if ((fbxNodeAttribute != nullptr) && (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton))
		{
			MeshSkeleton* newSkeleton = ImportSkeletonJoint(fbxSceneImport, matrixStack, currentSkeleton, parentJointIndex, reinterpret_cast<FbxSkeleton*>(fbxNodeAttribute), jointNodes);

			if (newSkeleton != nullptr)
			{
				currentSkeleton = newSkeleton;
				parentJointIndex = currentSkeleton->GetLastAddedJointIndex();
			}
		}
	}

	int childCount = fbxNode->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; ++childIndex)
	{
		ImportSkeletonNode(fbxSceneImport, fbxNode->GetChild(childIndex), matrixStack, currentSkeleton, jointNodes, parentJointIndex);
	}

	matrixStack.PopFromMatrixStack();
}



static void GetCurvesFromProperty(const FbxVector4& evaluatedTransform, FbxPropertyT<FbxDouble3> fbxProperty, FbxAnimLayer* fbxAnimationLayer, float startTime, float animationTime, TransformCurve* resultantTransformCurves)
{
	FbxAnimCurveNode* animationNode = fbxProperty.GetCurveNode();

	if ((animationNode == nullptr) || ((animationNode != nullptr) && (animationNode->GetChannelsCount() == 0)))
	{
		TransformCurve transformCurves[3];
		KeyFrame currentKeyFrame;

		currentKeyFrame.frameCurrentTime = 0.0f;
		currentKeyFrame.currentInterpolationType = CONSTANT_INTERPOLATION;

		currentKeyFrame.frameValue = static_cast<float>(evaluatedTransform.mData[0]);
		transformCurves[0].InsertKeyFrame(currentKeyFrame);
		currentKeyFrame.frameValue = static_cast<float>(evaluatedTransform.mData[1]);
		transformCurves[1].InsertKeyFrame(currentKeyFrame);
		currentKeyFrame.frameValue = static_cast<float>(evaluatedTransform.mData[2]);
		transformCurves[2].InsertKeyFrame(currentKeyFrame);

		resultantTransformCurves[0] = transformCurves[0];
		resultantTransformCurves[1] = transformCurves[1];
		resultantTransformCurves[2] = transformCurves[2];

		return;
	}

	std::string animationNodeComponents[3];

	animationNodeComponents[0] = FBXSDK_CURVENODE_COMPONENT_X;
	animationNodeComponents[1] = FBXSDK_CURVENODE_COMPONENT_Y;
	animationNodeComponents[2] = FBXSDK_CURVENODE_COMPONENT_Z;

	for (int componentIndex = 0; componentIndex < 3; ++componentIndex)
	{
		TransformCurve currentTransformCurve;
		FbxAnimCurve* animationCurve = fbxProperty.GetCurve(fbxAnimationLayer, animationNodeComponents[componentIndex].c_str());
		if (animationCurve != nullptr)
		{
			FbxTimeSpan curveTimeSpan;
			animationCurve->GetTimeInterval(curveTimeSpan);
			float curveStartTime = static_cast<float>(curveTimeSpan.GetStart().GetSecondDouble());
			float curveEndTime = static_cast<float>(curveTimeSpan.GetStop().GetSecondDouble());
			int numberOfKeyFrames = animationCurve->KeyGetCount();

			for (int keyFrameIndex = 0; keyFrameIndex < numberOfKeyFrames; ++keyFrameIndex)
			{
				KeyFrame currentKeyFrame;
				FbxAnimCurveKey fbxKeyFrame = animationCurve->KeyGet(keyFrameIndex);
				currentKeyFrame.frameValue = fbxKeyFrame.GetValue();

				float keyFrameTime = static_cast<float>(fbxKeyFrame.GetTime().GetSecondDouble());
				keyFrameTime = RangeMap(keyFrameTime, curveStartTime, curveEndTime, startTime, startTime + animationTime);
				currentKeyFrame.frameCurrentTime = keyFrameTime;

				switch (fbxKeyFrame.GetInterpolation())
				{
				case FbxAnimCurveDef::eInterpolationConstant:
					currentKeyFrame.currentInterpolationType = CONSTANT_INTERPOLATION;
					break;

				case FbxAnimCurveDef::eInterpolationLinear:
					currentKeyFrame.currentInterpolationType = LINEAR_INTERPOLATION;
					break;

				case FbxAnimCurveDef::eInterpolationCubic:
				{
					currentKeyFrame.currentInterpolationType = CUBIC_INTERPOLATION;

					switch (fbxKeyFrame.GetTangentMode())
					{
					case FbxAnimCurveDef::eTangentAuto:
						currentKeyFrame.positiveSlope = fbxKeyFrame.GetDataFloat(FbxAnimCurveDef::eRightVelocity);
						currentKeyFrame.negativeSlope = fbxKeyFrame.GetDataFloat(FbxAnimCurveDef::eNextLeftVelocity);
						break;

					default:
						ERROR_RECOVERABLE("Cubic Function is not supported.");
						break;
					}
				}

					break;

				default:
					ERROR_AND_DIE("Current KeyFrame is corrupted.");
				}

				currentTransformCurve.InsertKeyFrame(currentKeyFrame);
			}
		}
		else
		{
			KeyFrame currentKeyFrame;
			currentKeyFrame.frameCurrentTime = 0.0f;
			currentKeyFrame.currentInterpolationType = CONSTANT_INTERPOLATION;
			currentKeyFrame.frameValue = static_cast<float>(evaluatedTransform.mData[componentIndex]);
			
			currentTransformCurve.InsertKeyFrame(currentKeyFrame);
		}

		resultantTransformCurves[componentIndex] = currentTransformCurve;
	}
}



static void AddAnimationCurve(SkeletalAnimation* currentSkeletalAnimation, FbxNode* fbxNode, FbxAnimLayer* animationLayer)
{
	AnimationCurve* currentAnimationCurve = new AnimationCurve();

	FbxAnimCurveFilterUnroll animationCurveFilter;
	FbxAnimCurveNode* animationNode = fbxNode->LclRotation.GetCurveNode();
	if (animationNode != nullptr)
	{
		if (animationCurveFilter.NeedApply(*animationNode))
		{
			animationCurveFilter.SetQualityTolerance(0.25);
			animationCurveFilter.SetTestForPath(true);

			FbxTime fbxTime(0);
			animationCurveFilter.SetStartTime(fbxTime);
			ASSERT_OR_DIE(animationCurveFilter.Apply(*animationNode), "Filter cannot be applied.");
		}
	}

	TransformCurve scalingTransformCurves[3];
	GetCurvesFromProperty(fbxNode->EvaluateLocalScaling(), fbxNode->LclScaling, animationLayer, currentSkeletalAnimation->GetStartingTime(), currentSkeletalAnimation->GetAnimationDuration(), scalingTransformCurves);

	TransformCurve rotationTransformCurves[3];
	GetCurvesFromProperty(fbxNode->EvaluateLocalRotation(), fbxNode->LclRotation, animationLayer, currentSkeletalAnimation->GetStartingTime(), currentSkeletalAnimation->GetAnimationDuration(), rotationTransformCurves);

	TransformCurve translationTransformCurves[3];
	GetCurvesFromProperty(fbxNode->EvaluateLocalTranslation(), fbxNode->LclTranslation, animationLayer, currentSkeletalAnimation->GetStartingTime(), currentSkeletalAnimation->GetAnimationDuration(), translationTransformCurves);

	for (int curveIndex = 0; curveIndex < 3; ++curveIndex)
	{
		currentAnimationCurve->m_ScalingCurve[curveIndex] = scalingTransformCurves[curveIndex];
		currentAnimationCurve->m_RotationCurve[curveIndex] = rotationTransformCurves[curveIndex];
		currentAnimationCurve->m_TranslationCurve[curveIndex] = translationTransformCurves[curveIndex];
	}

	currentSkeletalAnimation->InsertAnimationCurve(*currentAnimationCurve);
}



static Matrix4 GetNodeWorldTransformAtTime(FbxNode* fbxNode, FbxTime fbxTime, const Matrix4& fbxImportTransform)
{
	Matrix4 nodeTransform;

	if (fbxNode == nullptr)
	{
		nodeTransform = Matrix4::IdentityMatrix4();
	}

	FbxMatrix fbxMatrix = fbxNode->EvaluateGlobalTransform(fbxTime);
	Matrix4 localMatrix = ConvertToLocalMatrix(fbxMatrix);

	nodeTransform = Matrix4::Multiply(localMatrix, fbxImportTransform);

	return nodeTransform;
}



static void ImportAnimations(SceneImport* fbxSceneImport, FbxScene* fbxScene, const Matrix4& fbxImportTransform, const std::vector<FbxNode*>& jointNodes, float frameRate)
{
	uint32_t numberOfAnimations = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	if (numberOfAnimations == 0)
	{
		return;
	}

	uint32_t numberOfSkeletons = fbxSceneImport->m_Skeletons.size();
	if (numberOfSkeletons == 0)
	{
		return;
	}

	FbxGlobalSettings& globalSettings = fbxScene->GetGlobalSettings();
	FbxTime::EMode timeMode = globalSettings.GetTimeMode();

	double fbxSceneFrameRate;
	if (timeMode == FbxTime::eCustom)
	{
		fbxSceneFrameRate = globalSettings.GetCustomFrameRate();
	}
	else
	{
		fbxSceneFrameRate = FbxTime::GetFrameRate(timeMode);
	}

	ASSERT_OR_DIE(numberOfSkeletons == 1, "FBX file should have only one skeleton.");
	MeshSkeleton* currentSkeleton = fbxSceneImport->m_Skeletons[0];

	FbxTime frameAdvanceTime;
	frameAdvanceTime.SetSecondDouble(static_cast<double>(1.0f / frameRate));

	for (uint32_t animationIndex = 0; animationIndex < numberOfAnimations; ++animationIndex)
	{
		FbxAnimStack* currentAnimationStack = fbxScene->GetSrcObject<FbxAnimStack>(animationIndex);
		if (currentAnimationStack == nullptr)
		{
			continue;
		}

		FbxTime localStartTime = currentAnimationStack->LocalStart;
		FbxTime startTime = (localStartTime.GetSecondDouble() > 0.0) ? localStartTime : FbxTime(0);
		FbxTime endTime = currentAnimationStack->LocalStop;
		FbxTime durationTime = endTime - startTime;

		fbxScene->SetCurrentAnimationStack(currentAnimationStack);

		std::string animationName = std::string(currentAnimationStack->GetName());

		float animationStartTime = static_cast<float>(startTime.GetSecondDouble());
		float animationDuration = static_cast<float>(durationTime.GetSecondDouble());

		SkeletalAnimation* skeletalAnimation = new SkeletalAnimation(animationName, animationStartTime, animationDuration, frameRate, currentSkeleton);

		uint32_t numberOfJoints = currentSkeleton->GetNumberOfJoints();
		for (uint32_t jointIndex = 0; jointIndex < numberOfJoints; ++jointIndex)
		{
			FbxNode* currentJointNode = jointNodes[jointIndex];
			Matrix4* jointKeyFrames = skeletalAnimation->GetKeyFramesForJointIndex(jointIndex);

			FbxTime evaluationTime = FbxTime(0);
			for (uint32_t frameIndex = 0; frameIndex < skeletalAnimation->GetNumberOfFrames(); ++frameIndex)
			{
				Matrix4* currentJointKeyFrame = jointKeyFrames + frameIndex;
				
				Matrix4 currentJointTransform = GetNodeWorldTransformAtTime(currentJointNode, evaluationTime, fbxImportTransform);
				*currentJointKeyFrame = currentJointTransform;

				evaluationTime += frameAdvanceTime;
			}
		}

		FbxAnimLayer* currentAnimationLayer = currentAnimationStack->GetMember<FbxAnimLayer>();
		for (FbxNode* currentJointNode : jointNodes)
		{
			AddAnimationCurve(skeletalAnimation, currentJointNode, currentAnimationLayer);
		}

		fbxSceneImport->m_Animations.push_back(skeletalAnimation);
	}
}



static void TriangulateScene(FbxScene* fbxScene)
{
	FbxGeometryConverter converter(fbxScene->GetFbxManager());
	converter.Triangulate(fbxScene, true);
}



static void ImportScene(SceneImport* fbxSceneImport, FbxScene* fbxScene, MatrixStack& matrixStack)
{
	TriangulateScene(fbxScene);

	FbxNode* rootNode = fbxScene->GetRootNode();

	std::vector<FbxNode*> jointNodes;
	ImportSkeletonNode(fbxSceneImport, rootNode, matrixStack, nullptr, jointNodes, -1);
	ImportMeshNode(fbxSceneImport, rootNode, matrixStack, jointNodes);

	Matrix4 topMatrix = matrixStack.GetTop();
	ImportAnimations(fbxSceneImport, fbxScene, topMatrix, jointNodes, 30.0f);
}



static Matrix4 GetFBXSceneBasis(FbxScene* fbxScene)
{
	FbxAxisSystem axisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();

	FbxAMatrix sceneMatrix;
	axisSystem.GetMatrix(sceneMatrix);

	Matrix4 fbxSceneBasis = ConvertToLocalMatrix(sceneMatrix);

	return fbxSceneBasis;
}



SceneImport* FBXLoadSceneFromFile(const std::string& fileName, const Matrix4& localBasis, const Matrix4& transformMatrix /*= Matrix4::IdentityMatrix4()*/, bool isRightHanded /*= true*/)
{
	FbxManager* fbxManager = FbxManager::Create();
	if (fbxManager == nullptr)
	{
		return nullptr;
	}

	FbxIOSettings* IOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	IOSettings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, false);
	fbxManager->SetIOSettings(IOSettings);

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool initializeResult = fbxImporter->Initialize(fileName.c_str(), -1, fbxManager->GetIOSettings());

	if (!initializeResult)
	{
		return nullptr;
	}

	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	bool importResult = fbxImporter->Import(fbxScene);

	if (!importResult)
	{
		return nullptr;
	}

	SceneImport* fbxSceneImport = new SceneImport();
	MatrixStack matrixStack;

	matrixStack.PushToMatrixStack(transformMatrix);
	matrixStack.PushToMatrixStack(localBasis);

	Matrix4 fbxSceneBasis = GetFBXSceneBasis(fbxScene);
	Matrix4 inverseFBXSceneBasis = fbxSceneBasis.GetTranspose();

	if (!isRightHanded)
	{

	}

	matrixStack.PushToMatrixStack(inverseFBXSceneBasis);

	ImportScene(fbxSceneImport, fbxScene, matrixStack);

	FBX_SAFE_DESTROY(fbxScene);
	FBX_SAFE_DESTROY(fbxImporter);
	FBX_SAFE_DESTROY(IOSettings);
	FBX_SAFE_DESTROY(fbxManager);

	return fbxSceneImport;
}



#else

void FBXListScene(const std::string& fileName)
{
	fileName;
}



SceneImport* FBXLoadSceneFromFile(const std::string& fileName, const Matrix4& localBasis, const Matrix4& transformMatrix /*= Matrix4::IdentityMatrix4()*/, bool isRightHanded /*= true*/)
{
	fileName;
	localBasis;
	transformMatrix;
	isRightHanded;
	return nullptr;
}

#endif // TOOLS_BUILD