#pragma once

#include <list>
#include <vector>

#include "Engine/Renderer/RenderUtilities/BasicRenderer.hpp"



enum DepthTestingMode
{
	ENABLED_DEPTH_TESTING,
	DISABLED_DEPTH_TESTING,
	DUAL_DEPTH_TESTING
};



struct RenderCommand
{
public:
	RenderCommand()
	{

	}

	RenderCommand(const std::vector<Vertex3D>& vertices, float lifeTimeInSeconds, unsigned int primitiveType, float thickness, DepthTestingMode depthTesting) :
	m_Vertices(vertices),
	m_LifeTimeInSeconds(lifeTimeInSeconds),
	m_ElapsedDurationInSeconds(0.0f),
	m_PrimitiveType(primitiveType),
	m_Thickness(thickness),
	m_DepthTesting(depthTesting)
	{

	}

	std::vector<Vertex3D> m_Vertices;
	float m_LifeTimeInSeconds;
	float m_ElapsedDurationInSeconds;
	unsigned int m_PrimitiveType;
	float m_Thickness;
	DepthTestingMode m_DepthTesting;
};



class DebugRenderer
{
public:
	DebugRenderer();
	~DebugRenderer();

	void Update(float deltaTimeInSeconds);
	void Render() const;

	void AddDebugPoint(const Vector3& pointPosition, const DepthTestingMode& depthTesting, const RGBA& pointColor = RGBA::WHITE, float pointLifeTime = FLT_MAX, float pointSize = 4.0f);
	void AddDebugLine(const Vector3& lineStartPosition, const Vector3& lineEndPosition, const DepthTestingMode& depthTesting, const RGBA& lineColor = RGBA::WHITE, float lineLifeTime = FLT_MAX, float lineThickness = 2.0f);
	void AddDebugArrow(const Vector3& arrowTailPosition, const Vector3& arrowHeadPosition, const DepthTestingMode& depthTesting, const RGBA& arrowColor = RGBA::WHITE, float arrowLifeTime = FLT_MAX, float arrowThickness = 2.0f);
	void AddDebugAABB3(const AABB3& boxBoundingPoints, const DepthTestingMode& depthTesting, float boxLifeTime = FLT_MAX, float lineThickness = 2.0f, const RGBA& boxColorEast = RGBA::RED, const RGBA& boxColorWest = RGBA::RED, const RGBA& boxColorNorth = RGBA::GREEN, const RGBA& boxColorSouth = RGBA::GREEN, const RGBA& boxColorTop = RGBA::BLUE, const RGBA& boxColorBottom = RGBA::BLUE);
	void AddDebugSphere(const Vector3& sphereCentrePosition, float sphereRadius, const DepthTestingMode& depthTesting, const RGBA& sphereColor = RGBA::WHITE, float sphereLifeTime = FLT_MAX, float sphereLineThickness = 2.0f);

	void ClearAllRenderCommands();

private:
	std::list<RenderCommand*> m_RenderCommands;
};

extern DebugRenderer* g_DebugRenderer;