#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderUtilities/BasicRenderer.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



BasicRenderer* g_BasicRenderer = nullptr;



BasicRenderer::BasicRenderer() :
MasterRenderer()
{
	InitializeRenderer();
}



BasicRenderer::~BasicRenderer()
{
	
}



void BasicRenderer::InitializeRenderer()
{
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

	glGenSamplers = (PFNGLGENSAMPLERSPROC)wglGetProcAddress("glGenSamplers");
	glBindSampler = (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
	glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress("glSamplerParameteri");
	glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)wglGetProcAddress("glDeleteSamplers");
}



void BasicRenderer::SetOrthographicProjection(const Vector2& bottomLeft, const Vector2& topRight)
{
	glLoadIdentity();
	glOrtho(bottomLeft.X, topRight.X, bottomLeft.Y, topRight.Y, 0.0f, 1.0f);
}



void BasicRenderer::SetPerspectiveProjection(float fovAngleInDegrees, float aspectRatio, float nearDistance, float farDistance)
{
	glLoadIdentity();
	gluPerspective(fovAngleInDegrees, aspectRatio, nearDistance, farDistance);
}



void BasicRenderer::TranslateViewMatrix(const Vector2& translatePosition)
{
	glTranslatef(translatePosition.X, translatePosition.Y, 0.0f);
}



void BasicRenderer::TranslateViewMatrix(const Vector3& translatePosition)
{
	glTranslatef(translatePosition.X, translatePosition.Y, translatePosition.Z);
}



void BasicRenderer::RotateViewMatrix(float rotateAngleInDegrees, const Vector3& axisOfRotation)
{
	glRotatef(rotateAngleInDegrees, axisOfRotation.X, axisOfRotation.Y, axisOfRotation.Z);
}



void BasicRenderer::RotateAroundOwnAxis(const Vector2& objectPosition, float rotateAngleInDegrees)
{
	glTranslatef(objectPosition.X, objectPosition.Y, 0.0f);
	glRotatef(rotateAngleInDegrees, 0.0f, 0.0f, 1.0f);
	glTranslatef(-objectPosition.X, -objectPosition.Y, 0.0f);
}



void BasicRenderer::ScaleViewMatrix(float scalingFactor)
{
	glScalef(scalingFactor, scalingFactor, 1.0f);
}



void BasicRenderer::PushViewMatrix()
{
	glPushMatrix();
}



void BasicRenderer::PopViewMatrix()
{
	glPopMatrix();
}



void BasicRenderer::Draw2DVertexArrays(const Vertex2D* vertices, int arraySize, unsigned int primitiveType)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &vertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2D), &vertices[0].m_Color);

	glDrawArrays(primitiveType, 0, arraySize);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}



void BasicRenderer::Draw3DVertexArrays(const Vertex3D* vertices, int arraySize, unsigned int primitiveType)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), &vertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D), &vertices[0].m_Color);

	glDrawArrays(primitiveType, 0, arraySize);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}



void BasicRenderer::Draw2DTexturedVertexArrays(const Vertex2D* vertices, int arraySize, unsigned int primitiveType, const Texture* texture)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->m_TextureID);
	glBindSampler(0, texture->m_SamplerID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &vertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2D), &vertices[0].m_Color);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex2D), &vertices[0].m_TextureCoordinates);

	glDrawArrays(primitiveType, 0, arraySize);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);
}



void BasicRenderer::Draw3DTexturedVertexArrays(const Vertex3D* vertices, int arraySize, unsigned int primitiveType, const Texture* texture)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->m_TextureID);
	glBindSampler(0, texture->m_SamplerID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), &vertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D), &vertices[0].m_Color);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), &vertices[0].m_TextureCoordinates);

	glDrawArrays(primitiveType, 0, arraySize);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);
}



void BasicRenderer::DrawSingle2DPointVA(const Vector2& pointPosition, float pointSize /*= 1.0f*/, const RGBA& pointColor /*= RGBA::WHITE*/)
{
	Vertex2D pointVertex[1];
	pointVertex[0].m_Position = pointPosition;
	pointVertex[0].m_Color = pointColor;

	Draw2DPointsVA(pointVertex, 1, pointSize);
}



void BasicRenderer::DrawSingle3DPointVA(const Vector3& pointPosition, float pointSize /*= 1.0f*/, const RGBA& pointColor /*= RGBA::WHITE*/)
{
	Vertex3D pointVertex[1];
	pointVertex[0].m_Position = pointPosition;
	pointVertex[0].m_Color = pointColor;

	Draw3DPointsVA(pointVertex, 1, pointSize);
}



void BasicRenderer::Draw2DPointsVA(const Vertex2D* pointVertices, int arraySize, float pointSize /*= 1.0f*/)
{
	SetPointSize(pointSize);
	Draw2DVertexArrays(pointVertices, arraySize, POINTS_PRIMITIVE);
}



void BasicRenderer::Draw3DPointsVA(const Vertex3D* pointVertices, int arraySize, float pointSize /*= 1.0f*/)
{
	SetPointSize(pointSize);
	Draw3DVertexArrays(pointVertices, arraySize, POINTS_PRIMITIVE);
}



void BasicRenderer::DrawSingle2DLineVA(const Vector2& startingPoint, const Vector2& endingPoint, float lineThickness /*= 1.0f*/, const RGBA& lineColor /*= RGBA::WHITE*/)
{
	Vertex2D lineVertices[2];

	lineVertices[0].m_Position = startingPoint;
	lineVertices[0].m_Color = lineColor;

	lineVertices[1].m_Position = endingPoint;
	lineVertices[1].m_Color = lineColor;

	Draw2DLinesVA(lineVertices, 2, lineThickness);
}



void BasicRenderer::DrawSingle3DLineVA(const Vector3& startingPoint, const Vector3& endingPoint, float lineThickness /*= 1.0f*/, const RGBA& lineColor /*= RGBA::WHITE*/)
{
	Vertex3D lineVertices[2];

	lineVertices[0].m_Position = startingPoint;
	lineVertices[0].m_Color = lineColor;

	lineVertices[1].m_Position = endingPoint;
	lineVertices[1].m_Color = lineColor;

	Draw3DLinesVA(lineVertices, 2, lineThickness);
}



void BasicRenderer::Draw2DLinesVA(const Vertex2D* lineVertices, int arraySize, float lineThickness /*= 1.0f*/)
{
	SetLineWidth(lineThickness);
	Draw2DVertexArrays(lineVertices, arraySize, LINES_PRIMITIVE);
}



void BasicRenderer::Draw3DLinesVA(const Vertex3D* lineVertices, int arraySize, float lineThickness /*= 1.0f*/)
{
	SetLineWidth(lineThickness);
	Draw3DVertexArrays(lineVertices, arraySize, LINES_PRIMITIVE);
}



void BasicRenderer::Draw2DOriginLines(const Vector2& originPoint, float lineLength, unsigned char lineAlpha /*= 255*/, float lineThickness /*= 1.0f*/)
{
	Vector2 yAxisPoint = Vector2(originPoint.X, originPoint.Y + lineLength);
	Vector2 xAxisPoint = Vector2(originPoint.X + lineLength, originPoint.Y);

	RGBA red = RGBA::RED;
	RGBA green = RGBA::GREEN;

	red.m_Alpha = lineAlpha;
	green.m_Alpha = lineAlpha;

	DrawSingle2DLineVA(originPoint, xAxisPoint, lineThickness, red);
	DrawSingle2DLineVA(originPoint, yAxisPoint, lineThickness, green);
}



void BasicRenderer::Draw3DOriginLines(const Vector3& originPoint, float lineLength, unsigned char lineAlpha /*= 255*/, float lineThickness /*= 1.0f*/)
{
	Vector3 xAxisPoint = Vector3(originPoint.X + lineLength, originPoint.Y, originPoint.Z);
	Vector3 yAxisPoint = Vector3(originPoint.X, originPoint.Y + lineLength, originPoint.Z);
	Vector3 zAxisPoint = Vector3(originPoint.X, originPoint.Y, originPoint.Z + lineLength);

	RGBA red = RGBA::RED;
	RGBA green = RGBA::GREEN;
	RGBA blue = RGBA::BLUE;

	red.m_Alpha = lineAlpha;
	green.m_Alpha = lineAlpha;
	blue.m_Alpha = lineAlpha;

	DrawSingle3DLineVA(originPoint, xAxisPoint, lineThickness, red);
	DrawSingle3DLineVA(originPoint, yAxisPoint, lineThickness, green);
	DrawSingle3DLineVA(originPoint, zAxisPoint, lineThickness, blue);
}



void BasicRenderer::Draw2DGrid(const AABB2& boundingPoints, const RGBA& gridColor /*= RGBA::WHITE*/)
{
	for (float X = boundingPoints.minimums.X; X <= boundingPoints.maximums.X; ++X)
	{
		DrawSingle2DLineVA(Vector2(X, boundingPoints.minimums.Y), Vector2(X, boundingPoints.maximums.Y), 1.0f, gridColor);
	}
	for (float Y = boundingPoints.minimums.Y; Y <= boundingPoints.maximums.Y; ++Y)
	{
		DrawSingle2DLineVA(Vector2(boundingPoints.minimums.X, Y), Vector2(boundingPoints.maximums.X, Y), 1.0f, gridColor);
	}
}



void BasicRenderer::Draw3DGrid(const AABB3& boundingPoints, const RGBA& gridColor /*= RGBA::WHITE*/)
{
	for (float X = boundingPoints.minimums.X; X <= boundingPoints.maximums.X; ++X)
	{
		for (float Y = boundingPoints.minimums.Y; Y <= boundingPoints.maximums.Y; ++Y)
		{
			for (float Z = boundingPoints.minimums.Z; Z <= boundingPoints.maximums.Z; ++Z)
			{
				DrawSingle3DLineVA(Vector3(boundingPoints.minimums.X, Y, Z), Vector3(boundingPoints.maximums.X, Y, Z), 1.0f, gridColor);
				DrawSingle3DLineVA(Vector3(X, boundingPoints.minimums.Y, Z), Vector3(X, boundingPoints.maximums.Y, Z), 1.0f, gridColor);
				DrawSingle3DLineVA(Vector3(X, Y, boundingPoints.minimums.Z), Vector3(X, Y, boundingPoints.maximums.Z), 1.0f, gridColor);
			}
		}
	}
}



void BasicRenderer::DrawSingle2DQuadVA(const AABB2& boundingPoints, bool wireframeMode /*= false*/, const RGBA& quadColor /*= RGBA::WHITE*/)
{
	Vertex2D quadVertices[4];

	quadVertices[0].m_Position = Vector2(boundingPoints.minimums.X, boundingPoints.minimums.Y);
	quadVertices[0].m_Color = quadColor;

	quadVertices[1].m_Position = Vector2(boundingPoints.maximums.X, boundingPoints.minimums.Y);
	quadVertices[1].m_Color = quadColor;

	quadVertices[2].m_Position = Vector2(boundingPoints.maximums.X, boundingPoints.maximums.Y);
	quadVertices[2].m_Color = quadColor;

	quadVertices[3].m_Position = Vector2(boundingPoints.minimums.X, boundingPoints.maximums.Y);
	quadVertices[3].m_Color = quadColor;

	Draw2DQuadsVA(quadVertices, 4, wireframeMode);
}



void BasicRenderer::DrawSingle3DQuadVA(const Vector3& bottomLeftPoint, const Vector3& bottomRightPoint, const Vector3& topLeftPoint, const Vector3& topRightPoint, bool wireframeMode /*= false*/, const RGBA& quadColor /*= RGBA::WHITE*/)
{
	Vertex3D quadVertices[4];

	quadVertices[0].m_Position = bottomLeftPoint;
	quadVertices[0].m_Color = quadColor;

	quadVertices[1].m_Position = bottomRightPoint;
	quadVertices[1].m_Color = quadColor;

	quadVertices[2].m_Position = topRightPoint;
	quadVertices[2].m_Color = quadColor;

	quadVertices[3].m_Position = topLeftPoint;
	quadVertices[3].m_Color = quadColor;

	Draw3DQuadsVA(quadVertices, 4, wireframeMode);
}



void BasicRenderer::Draw2DQuadsVA(const Vertex2D* quadVertices, int arraySize, bool wireframeMode /*= false*/)
{
	unsigned int primitiveType = (wireframeMode) ? LINE_LOOP_PRIMITIVE : QUADS_PRIMITIVE;
	if (wireframeMode)
	{
		SetLineWidth(2.0f);
	}

	Draw2DVertexArrays(quadVertices, arraySize, primitiveType);
}



void BasicRenderer::Draw3DQuadsVA(const Vertex3D* quadVertices, int arraySize, bool wireframeMode /*= false*/)
{
	unsigned int primitiveType = (wireframeMode) ? LINE_LOOP_PRIMITIVE : QUADS_PRIMITIVE;
	if (wireframeMode)
	{
		SetLineWidth(2.0f);
	}

	Draw3DVertexArrays(quadVertices, arraySize, primitiveType);
}



void BasicRenderer::DrawSingle2DTexturedQuadVA(const AABB2& boundingPoints, const AABB2& textureBounds, const Texture* texture, const RGBA& textureTint /*= RGBA::WHITE*/)
{
	Vertex2D quadVertices[4];

	quadVertices[0].m_Position = Vector2(boundingPoints.minimums.X, boundingPoints.minimums.Y);
	quadVertices[0].m_Color = textureTint;
	quadVertices[0].m_TextureCoordinates = Vector2(textureBounds.minimums.X, textureBounds.maximums.Y);

	quadVertices[1].m_Position = Vector2(boundingPoints.maximums.X, boundingPoints.minimums.Y);
	quadVertices[1].m_Color = textureTint;
	quadVertices[1].m_TextureCoordinates = Vector2(textureBounds.maximums.X, textureBounds.maximums.Y);

	quadVertices[2].m_Position = Vector2(boundingPoints.maximums.X, boundingPoints.maximums.Y);
	quadVertices[2].m_Color = textureTint;
	quadVertices[2].m_TextureCoordinates = Vector2(textureBounds.maximums.X, textureBounds.minimums.Y);

	quadVertices[3].m_Position = Vector2(boundingPoints.minimums.X, boundingPoints.maximums.Y);
	quadVertices[3].m_Color = textureTint;
	quadVertices[3].m_TextureCoordinates = Vector2(textureBounds.minimums.X, textureBounds.minimums.Y);

	Draw2DTexturedQuadsVA(quadVertices, 4, texture);
}



void BasicRenderer::DrawSingle3DTexturedQuadVA(const Vector3& bottomLeftPoint, const Vector3& bottomRightPoint, const Vector3& topLeftPoint, const Vector3& topRightPoint, const AABB2& textureBounds, const Texture* texture, const RGBA& textureTint /*= RGBA::WHITE*/)
{
	Vertex3D quadVertices[4];

	quadVertices[0].m_Position = bottomLeftPoint;
	quadVertices[0].m_Color = textureTint;
	quadVertices[0].m_TextureCoordinates = Vector2(textureBounds.minimums.X, textureBounds.maximums.Y);

	quadVertices[1].m_Position = bottomRightPoint;
	quadVertices[1].m_Color = textureTint;
	quadVertices[1].m_TextureCoordinates = Vector2(textureBounds.maximums.X, textureBounds.maximums.Y);

	quadVertices[2].m_Position = topRightPoint;
	quadVertices[2].m_Color = textureTint;
	quadVertices[2].m_TextureCoordinates = Vector2(textureBounds.maximums.X, textureBounds.minimums.Y);

	quadVertices[3].m_Position = topLeftPoint;
	quadVertices[3].m_Color = textureTint;
	quadVertices[3].m_TextureCoordinates = Vector2(textureBounds.minimums.X, textureBounds.minimums.Y);

	Draw3DTexturedQuadsVA(quadVertices, 4, texture);
}



void BasicRenderer::Draw2DTexturedQuadsVA(const Vertex2D* quadVertices, int arraySize, const Texture* texture)
{
	Draw2DTexturedVertexArrays(quadVertices, arraySize, QUADS_PRIMITIVE, texture);
}



void BasicRenderer::Draw3DTexturedQuadsVA(const Vertex3D* quadVertices, int arraySize, const Texture* texture)
{
	Draw3DTexturedVertexArrays(quadVertices, arraySize, QUADS_PRIMITIVE, texture);
}



void BasicRenderer::Draw3DSphereVA(const Vector3& sphereCentrePosition, float sphereRadius, bool wireframeMode /*= false*/, const RGBA& sphereColor /*= RGBA::WHITE*/)
{
	if (wireframeMode)
	{
		SetLineWidth(2.0f);

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

			Draw3DVertexArrays(sphereLongitudeVertices.data(), sphereLongitudeVertices.size(), LINE_LOOP_PRIMITIVE);
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

			Draw3DVertexArrays(sphereLatitudeVertices.data(), sphereLatitudeVertices.size(), LINE_LOOP_PRIMITIVE);
		}
	}
	else
	{
		std::vector<Vertex3D> sphereVertices;
		Vertex3D sphereVertex;
		sphereVertex.m_Color = sphereColor;

		std::vector<unsigned short> sphereIndices;
		unsigned short sphereIndex = 0;

		for (float yawAngle = 0.0f; yawAngle <= 360.0f; yawAngle += 10.0f)
		{
			Vector3 sphereVertexBaseOffset;
			sphereVertexBaseOffset.X = sphereRadius * CosineOfDegrees(yawAngle);
			sphereVertexBaseOffset.Y = sphereRadius * SineOfDegrees(yawAngle);
			sphereVertexBaseOffset.Z = sphereRadius;

			for (float pitchAngle = -90.0f; pitchAngle <= 90.0f; pitchAngle += 10.0f)
			{
				Vector3 sphereVertexOffset;
				sphereVertexOffset.X = sphereVertexBaseOffset.X * CosineOfDegrees(pitchAngle);
				sphereVertexOffset.Y = sphereVertexBaseOffset.Y * CosineOfDegrees(pitchAngle);
				sphereVertexOffset.Z = sphereVertexBaseOffset.Z * SineOfDegrees(-pitchAngle);

				sphereVertex.m_Position = sphereCentrePosition + sphereVertexOffset;
				sphereVertices.push_back(sphereVertex);

				if (pitchAngle < 90.0f)
				{
					if (yawAngle < 360.0f)
					{
						sphereIndices.push_back(sphereIndex);
						sphereIndices.push_back(sphereIndex + 1);
						sphereIndices.push_back(sphereIndex + 19 + 1);
						sphereIndices.push_back(sphereIndex + 19);
					}
				}
				++sphereIndex;
			}
		}

		DrawVertexElementQuads(sphereVertices, sphereIndices);
	}
}



void BasicRenderer::Draw3DTexturedSphereVA(const Vector3& sphereCentrePosition, float sphereRadius, const Texture* texture, const RGBA& textureTint /*= RGBA::WHITE*/)
{
	std::vector<Vertex3D> sphereVertices;
	Vertex3D sphereVertex;
	sphereVertex.m_Color = textureTint;

	std::vector<unsigned short> sphereIndices;
	unsigned short sphereIndex = 0;

	float IncrementX = 10.0f / 360.0f;
	float IncrementY = 10.0f / 180.0f;

	float TextureCoordinateX = 0.0f;

	for (float yawAngle = 0.0f; yawAngle <= 360.0f; yawAngle += 10.0f)
	{
		float TextureCoordinateY = 0.0f;

		Vector3 sphereVertexBaseOffset;
		sphereVertexBaseOffset.X = sphereRadius * CosineOfDegrees(yawAngle);
		sphereVertexBaseOffset.Y = sphereRadius * SineOfDegrees(yawAngle);
		sphereVertexBaseOffset.Z = sphereRadius;

		for (float pitchAngle = -90.0f; pitchAngle <= 90.0f; pitchAngle += 10.0f)
		{
			Vector3 sphereVertexOffset;
			sphereVertexOffset.X = sphereVertexBaseOffset.X * CosineOfDegrees(pitchAngle);
			sphereVertexOffset.Y = sphereVertexBaseOffset.Y * CosineOfDegrees(pitchAngle);
			sphereVertexOffset.Z = sphereVertexBaseOffset.Z * SineOfDegrees(-pitchAngle);

			if (pitchAngle < 90.0f)
			{
				if (yawAngle < 360.0f)
				{
					sphereIndices.push_back(sphereIndex);
					sphereIndices.push_back(sphereIndex + 1);
					sphereIndices.push_back(sphereIndex + 19 + 1);
					sphereIndices.push_back(sphereIndex + 19);
				}
			}
			++sphereIndex;

			sphereVertex.m_Position = sphereCentrePosition + sphereVertexOffset;
			sphereVertex.m_TextureCoordinates = Vector2(TextureCoordinateX, TextureCoordinateY);
			sphereVertices.push_back(sphereVertex);

			TextureCoordinateY += IncrementY;
		}

		TextureCoordinateX += IncrementX;
	}

	DrawTexturedVertexElementQuads(sphereVertices, sphereIndices, texture);
}



void BasicRenderer::DrawVertexElementQuads(const std::vector<Vertex3D>& quadVertices, const std::vector<unsigned short>& quadIndices, bool wireframeMode /*= false*/)
{
	unsigned int drawMode = (wireframeMode) ? GL_LINES : GL_QUADS;
	if (wireframeMode)
	{
		SetLineWidth(2.0f);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), &quadVertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D), &quadVertices[0].m_Color);

	glDrawElements(drawMode, quadIndices.size(), GL_UNSIGNED_SHORT, &quadIndices[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}



void BasicRenderer::DrawTexturedVertexElementQuads(const std::vector<Vertex3D>& quadVertices, const std::vector<unsigned short>& quadIndices, const Texture* texture)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->m_TextureID);
	glBindSampler(0, texture->m_SamplerID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), &quadVertices[0].m_Position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D), &quadVertices[0].m_Color);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), &quadVertices[0].m_TextureCoordinates);

	glDrawElements(GL_QUADS, quadIndices.size(), GL_UNSIGNED_SHORT, &quadIndices[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);
}



void BasicRenderer::Draw2DMonospacedText(const Vector2& startingLetterMinimums, const std::string& asciiText, float cellHeight, float cellAspectRatio, const MonospaceFont* font /*= nullptr*/, const RGBA& textTint /*= RGBA::WHITE*/)
{
	Texture* glyphTexture = font->GetFontTexture();

	float cellWidth = cellHeight * cellAspectRatio;
	Vector2 cellDimensions(cellWidth, cellHeight);
	Vector2 letterMinimums = startingLetterMinimums;

	for (char asciiGlyph : asciiText)
	{
		if (asciiGlyph == '\n')
		{
			letterMinimums.X = startingLetterMinimums.X;
			letterMinimums.Y -= cellHeight;
			continue;
		}

		AABB2 glyphTextureCoordinates = font->GetTextureCoordsForGlyph(asciiGlyph);

		Vector2 letterMaximums = letterMinimums + cellDimensions;
		AABB2 letterBounds = AABB2(letterMinimums, letterMaximums);
		DrawSingle2DTexturedQuadVA(letterBounds, glyphTextureCoordinates, glyphTexture, textTint);

		letterMinimums.X += cellWidth;
	}
}



void BasicRenderer::Draw2DProportionalText(const Vector2& startingLetterMinimums, const std::string& asciiText, float cellHeight, const ProportionalFont* font /*= nullptr*/, const RGBA& textTint /*= RGBA::WHITE*/)
{
	Vector2 cursorPosition = startingLetterMinimums;
	cursorPosition.Y += cellHeight;

	float textScale = font->GetCharacterScaleForCellHeight(cellHeight);

	ProportionalGlyph* previousGlyph = nullptr;

	for (char asciiGlyph : asciiText)
	{
		ProportionalGlyph* currentGlyph = font->GetProportionalGlyphForGlyphUnicode(asciiGlyph);
		if (currentGlyph == nullptr)
		{
			continue;
		}

		float currentGlyphKerning = 0.0f;
		if (previousGlyph != nullptr && currentGlyph != nullptr)
		{
			currentGlyphKerning = font->GetKerningForGlyphPair(previousGlyph->m_glyphUnicode, currentGlyph->m_glyphUnicode);
		}

		cursorPosition.X += currentGlyphKerning * textScale;

		Vector2 glyphOffset = currentGlyph->GetOffset();
		Vector2 glyphSize = currentGlyph->GetSize();

		AABB2 glyphBounds;

		glyphBounds.minimums.X = cursorPosition.X + (glyphOffset.X * textScale);
		glyphBounds.maximums.Y = cursorPosition.Y - (glyphOffset.Y * textScale);

		glyphBounds.maximums.X = glyphBounds.minimums.X + (glyphSize.X * textScale);
		glyphBounds.minimums.Y = glyphBounds.maximums.Y - (glyphSize.Y * textScale);

		AABB2 glyphTextureBounds = font->GetTextureBoundsForGlyph(*currentGlyph);
		Texture* glyphTexture = font->GetFontTextureForGlyph(*currentGlyph);

		DrawSingle2DTexturedQuadVA(glyphBounds, glyphTextureBounds, glyphTexture, textTint);

		cursorPosition.X += currentGlyph->GetAdvance().X * textScale;
		previousGlyph = currentGlyph;
	}
}