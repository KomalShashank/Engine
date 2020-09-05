#include "Engine/Renderer/RenderUtilities/DebugRenderer.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



DebugRenderer* g_DebugRenderer = nullptr;



DebugRenderer::DebugRenderer()
{

}



DebugRenderer::~DebugRenderer()
{
	for (RenderCommand* currentRenderCommand : m_RenderCommands)
	{
		delete currentRenderCommand;
	}
}



void DebugRenderer::Update(float deltaTimeInSeconds)
{
	for (auto currentRenderCommandIterator = m_RenderCommands.begin(); currentRenderCommandIterator != m_RenderCommands.end();)
	{
		RenderCommand* currentRenderCommand = *currentRenderCommandIterator;
		if (currentRenderCommand->m_LifeTimeInSeconds != FLT_MAX)
		{
			currentRenderCommand->m_ElapsedDurationInSeconds += deltaTimeInSeconds;
			if (currentRenderCommand->m_ElapsedDurationInSeconds > currentRenderCommand->m_LifeTimeInSeconds)
			{
				currentRenderCommandIterator = m_RenderCommands.erase(currentRenderCommandIterator);
				continue;
			}
		}

		++currentRenderCommandIterator;
	}
}



void DebugRenderer::Render() const
{
	for (RenderCommand* currentRenderCommand : m_RenderCommands)
	{
		if (currentRenderCommand->m_DepthTesting == ENABLED_DEPTH_TESTING)
		{
			g_BasicRenderer->EnableDepthTesting(true);
			g_BasicRenderer->SetPointSize(currentRenderCommand->m_Thickness);
			g_BasicRenderer->SetLineWidth(currentRenderCommand->m_Thickness);
			g_BasicRenderer->Draw3DVertexArrays(currentRenderCommand->m_Vertices.data(), currentRenderCommand->m_Vertices.size(), currentRenderCommand->m_PrimitiveType);
		}
		else if (currentRenderCommand->m_DepthTesting == DISABLED_DEPTH_TESTING)
		{
			g_BasicRenderer->EnableDepthTesting(false);
			g_BasicRenderer->SetPointSize(currentRenderCommand->m_Thickness);
			g_BasicRenderer->SetLineWidth(currentRenderCommand->m_Thickness);
			g_BasicRenderer->Draw3DVertexArrays(currentRenderCommand->m_Vertices.data(), currentRenderCommand->m_Vertices.size(), currentRenderCommand->m_PrimitiveType);
			g_BasicRenderer->EnableDepthTesting(true);
		}
		else if (currentRenderCommand->m_DepthTesting == DUAL_DEPTH_TESTING)
		{
			g_BasicRenderer->EnableDepthTesting(true);
			g_BasicRenderer->SetPointSize(currentRenderCommand->m_Thickness);
			g_BasicRenderer->SetLineWidth(currentRenderCommand->m_Thickness);
			g_BasicRenderer->Draw3DVertexArrays(currentRenderCommand->m_Vertices.data(), currentRenderCommand->m_Vertices.size(), currentRenderCommand->m_PrimitiveType);
			g_BasicRenderer->EnableDepthTesting(false);
			g_BasicRenderer->SetPointSize(currentRenderCommand->m_Thickness / 2.0f);
			g_BasicRenderer->SetLineWidth(currentRenderCommand->m_Thickness / 2.0f);
			g_BasicRenderer->Draw3DVertexArrays(currentRenderCommand->m_Vertices.data(), currentRenderCommand->m_Vertices.size(), currentRenderCommand->m_PrimitiveType);
			g_BasicRenderer->EnableDepthTesting(true);
		}
	}
}



void DebugRenderer::AddDebugPoint(const Vector3& pointPosition, const DepthTestingMode& depthTesting, const RGBA& pointColor /*= RGBA::WHITE*/, float pointLifeTime /*= FLT_MAX*/, float pointSize /*= 4.0f*/)
{
	std::vector<Vertex3D> pointVertices;
	Vertex3D pointVertex;

	pointVertex.m_Color = pointColor;

	pointVertex.m_Position = pointPosition;
	pointVertices.push_back(pointVertex);

	RenderCommand* pointRenderCommand = new RenderCommand(pointVertices, pointLifeTime, POINTS_PRIMITIVE, pointSize, depthTesting);
	m_RenderCommands.push_back(pointRenderCommand);



	std::vector<Vertex3D> asteriskVertices;
	Vertex3D asteriskVertex;

	asteriskVertex.m_Color = pointColor;

	asteriskVertex.m_Position = pointPosition - (Vector3::X_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	asteriskVertex.m_Position = pointPosition + (Vector3::X_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	asteriskVertex.m_Position = pointPosition - (Vector3::Y_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	asteriskVertex.m_Position = pointPosition + (Vector3::Y_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	asteriskVertex.m_Position = pointPosition - (Vector3::Z_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	asteriskVertex.m_Position = pointPosition + (Vector3::Z_AXIS * 0.5f);
	asteriskVertices.push_back(asteriskVertex);

	RenderCommand* asteriskRenderCommand = new RenderCommand(asteriskVertices, pointLifeTime, LINES_PRIMITIVE, pointSize / 2.0f, depthTesting);
	m_RenderCommands.push_back(asteriskRenderCommand);
}



void DebugRenderer::AddDebugLine(const Vector3& lineStartPosition, const Vector3& lineEndPosition, const DepthTestingMode& depthTesting, const RGBA& lineColor /*= RGBA::WHITE*/, float lineLifeTime /*= FLT_MAX*/, float lineThickness /*= 2.0f*/)
{
	std::vector<Vertex3D> lineVertices;
	Vertex3D lineVertex;

	lineVertex.m_Color = lineColor;

	lineVertex.m_Position = lineStartPosition;
	lineVertices.push_back(lineVertex);

	lineVertex.m_Position = lineEndPosition;
	lineVertices.push_back(lineVertex);

	RenderCommand* lineRenderCommand = new RenderCommand(lineVertices, lineLifeTime, LINES_PRIMITIVE, lineThickness, depthTesting);
	m_RenderCommands.push_back(lineRenderCommand);
}



void DebugRenderer::AddDebugArrow(const Vector3& arrowTailPosition, const Vector3& arrowHeadPosition, const DepthTestingMode& depthTesting, const RGBA& arrowColor /*= RGBA::WHITE*/, float arrowLifeTime /*= FLT_MAX*/, float arrowThickness /*= 2.0f*/)
{
	Vector3 arrowDirection = arrowHeadPosition - arrowTailPosition;

	std::vector<Vertex3D> arrowBodyVertices;
	Vertex3D arrowBodyVertex;

	arrowBodyVertex.m_Color = arrowColor;

	arrowBodyVertex.m_Position = arrowTailPosition;
	arrowBodyVertices.push_back(arrowBodyVertex);

	arrowBodyVertex.m_Position = arrowTailPosition + (arrowDirection * 0.9f);
	arrowBodyVertices.push_back(arrowBodyVertex);

	RenderCommand* arrowBodyRenderCommand = new RenderCommand(arrowBodyVertices, arrowLifeTime, LINES_PRIMITIVE, arrowThickness, depthTesting);
	m_RenderCommands.push_back(arrowBodyRenderCommand);



	std::vector<Vertex3D> arrowHeadVertices;
	Vertex3D arrowHeadVertex;

	arrowHeadVertex.m_Color = arrowColor;

	arrowHeadVertex.m_Position = arrowTailPosition + (arrowDirection * 0.9f);
	arrowHeadVertices.push_back(arrowHeadVertex);

	arrowHeadVertex.m_Position = arrowHeadPosition;
	arrowHeadVertices.push_back(arrowHeadVertex);

	RenderCommand* arrowHeadRenderCommand = new RenderCommand(arrowHeadVertices, arrowLifeTime, LINES_PRIMITIVE, arrowThickness * 4.0f, depthTesting);
	m_RenderCommands.push_back(arrowHeadRenderCommand);
}



void DebugRenderer::AddDebugAABB3(const AABB3& boxBoundingPoints, const DepthTestingMode& depthTesting, float boxLifeTime /*= FLT_MAX*/, float lineThickness /*= 2.0f*/, const RGBA& boxColorEast /*= RGBA::RED*/, const RGBA& boxColorWest /*= RGBA::RED*/, const RGBA& boxColorNorth /*= RGBA::GREEN*/, const RGBA& boxColorSouth /*= RGBA::GREEN*/, const RGBA& boxColorTop /*= RGBA::BLUE*/, const RGBA& boxColorBottom /*= RGBA::BLUE*/)
{
	std::vector<Vertex3D> boxVertices;
	Vertex3D boxVertex;

	boxVertex.m_Color = boxColorEast;
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);

	boxVertex.m_Color = boxColorWest;
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);

	boxVertex.m_Color = boxColorNorth;
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);

	boxVertex.m_Color = boxColorSouth;
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);

	boxVertex.m_Color = boxColorTop;
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.maximums.Z);
	boxVertices.push_back(boxVertex);

	boxVertex.m_Color = boxColorBottom;
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.maximums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.maximums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);
	boxVertex.m_Position = Vector3(boxBoundingPoints.minimums.X, boxBoundingPoints.minimums.Y, boxBoundingPoints.minimums.Z);
	boxVertices.push_back(boxVertex);

	RenderCommand* boxRenderCommand = new RenderCommand(boxVertices, boxLifeTime, QUADS_PRIMITIVE, lineThickness, depthTesting);
	m_RenderCommands.push_back(boxRenderCommand);
}



void DebugRenderer::AddDebugSphere(const Vector3& sphereCentrePosition, float sphereRadius, const DepthTestingMode& depthTesting, const RGBA& sphereColor /*= RGBA::WHITE*/, float sphereLifeTime /*= FLT_MAX*/, float sphereLineThickness /*= 2.0f*/)
{
	for (float yawAngle = 0.0f; yawAngle < 360.0f; yawAngle += 10.0f)
	{
		std::vector<Vertex3D> sphereLongitudeVertices;
		Vertex3D sphereLongitudeVertex;

		sphereLongitudeVertex.m_Color = sphereColor;

		Vector3 sphereLongitudeBaseOffset;
		sphereLongitudeBaseOffset.X = sphereRadius * CosineOfDegrees(yawAngle);
		sphereLongitudeBaseOffset.Y = sphereRadius * SineOfDegrees(yawAngle);
		sphereLongitudeBaseOffset.Z = sphereRadius;

		for (float pitchAngle = 0.0f; pitchAngle < 360.0f; pitchAngle += 10.0f)
		{
			Vector3 sphereLongitudeOffset;
			sphereLongitudeOffset.X = sphereLongitudeBaseOffset.X * CosineOfDegrees(pitchAngle);
			sphereLongitudeOffset.Y = sphereLongitudeBaseOffset.Y * CosineOfDegrees(pitchAngle);
			sphereLongitudeOffset.Z = sphereLongitudeBaseOffset.Z * SineOfDegrees(-pitchAngle);

			sphereLongitudeVertex.m_Position = sphereCentrePosition + sphereLongitudeOffset;
			sphereLongitudeVertices.push_back(sphereLongitudeVertex);
		}

		RenderCommand* sphereLongitudeRenderCommand = new RenderCommand(sphereLongitudeVertices, sphereLifeTime, LINE_LOOP_PRIMITIVE, sphereLineThickness, depthTesting);
		m_RenderCommands.push_back(sphereLongitudeRenderCommand);
	}

	for (float pitchAngle = -90.0f; pitchAngle <= 90.0f; pitchAngle += 10.0f)
	{
		std::vector<Vertex3D> sphereLatitudeVertices;
		Vertex3D sphereLatitudeVertex;

		sphereLatitudeVertex.m_Color = sphereColor;

		Vector3 sphereLatitudeBaseOffset;
		sphereLatitudeBaseOffset.X = sphereRadius * CosineOfDegrees(pitchAngle);
		sphereLatitudeBaseOffset.Y = sphereRadius * CosineOfDegrees(pitchAngle);
		sphereLatitudeBaseOffset.Z = sphereRadius * SineOfDegrees(-pitchAngle);
		
		for (float yawAngle = 0.0f; yawAngle < 360.0f; yawAngle += 10.0f)
		{
			Vector3 sphereLatitudeOffset;
			sphereLatitudeOffset.X = sphereLatitudeBaseOffset.X * CosineOfDegrees(yawAngle);
			sphereLatitudeOffset.Y = sphereLatitudeBaseOffset.Y * SineOfDegrees(yawAngle);
			sphereLatitudeOffset.Z = sphereLatitudeBaseOffset.Z;

			sphereLatitudeVertex.m_Position = sphereCentrePosition + sphereLatitudeOffset;
			sphereLatitudeVertices.push_back(sphereLatitudeVertex);
		}

		RenderCommand* sphereLatitudeRenderCommand = new RenderCommand(sphereLatitudeVertices, sphereLifeTime, LINE_LOOP_PRIMITIVE, sphereLineThickness, depthTesting);
		m_RenderCommands.push_back(sphereLatitudeRenderCommand);
	}
}



void DebugRenderer::ClearAllRenderCommands()
{
	m_RenderCommands.clear();
}