#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



AdvancedRenderer* g_AdvancedRenderer = nullptr;



AdvancedRenderer::AdvancedRenderer() :
MasterRenderer(),
m_WindowDimensions(IntVector2::ZERO)
{
	InitializeOpenGL();

	SamplerData textureSamplerData = SamplerData(REPEAT_WRAP, REPEAT_WRAP, NEAREST_FILTER, NEAREST_FILTER);
	m_DefaultTexture = Texture::CreateDefaultTexture(IntVector2(2, 2), RGBA_8, textureSamplerData, true);

	m_DefaultMaterial = new Material("Data/Shaders/DefaultShader.vert", "Data/Shaders/DefaultShader.frag");
	m_DefaultMaterial->SetDiffuseTexture(m_DefaultTexture);

	m_DefaultMesh = new Mesh();
	m_MeshRenderer = new MeshRenderer(m_DefaultMesh, m_DefaultMaterial);
}



AdvancedRenderer::~AdvancedRenderer()
{
	delete m_DefaultMesh;
	delete m_DefaultMaterial;
	delete m_DefaultTexture;
	delete m_MeshRenderer;
}



void AdvancedRenderer::InitializeOpenGL()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");

	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");

	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

	glGenSamplers = (PFNGLGENSAMPLERSPROC)wglGetProcAddress("glGenSamplers");
	glBindSampler = (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
	glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress("glSamplerParameteri");
	glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)wglGetProcAddress("glDeleteSamplers");

	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
	glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");

	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
}



void AdvancedRenderer::InitializeRenderer(const IntVector2& windowDimensions)
{
	if (g_AdvancedRenderer == nullptr)
	{
		g_AdvancedRenderer = new AdvancedRenderer();
	}

	g_AdvancedRenderer->m_WindowDimensions = windowDimensions;
}



void AdvancedRenderer::UninitializeRenderer()
{
	if (g_AdvancedRenderer != nullptr)
	{
		delete g_AdvancedRenderer;
		g_AdvancedRenderer = nullptr;
	}
}



AdvancedRenderer* AdvancedRenderer::SingletonInstance()
{
	ASSERT_OR_DIE(g_AdvancedRenderer != nullptr, "Renderer instance does not exist.");

	return g_AdvancedRenderer;
}



void AdvancedRenderer::UpdateModelMatrix(const Matrix4& modelMatrix)
{
	m_ModelMatrix = modelMatrix;
}



void AdvancedRenderer::UpdateViewMatrix(const Matrix4& viewMatrix)
{
	m_ViewMatrix = viewMatrix;
}



void AdvancedRenderer::UpdateProjectionMatrix(const Matrix4& projectionMatrix)
{
	m_ProjectionMatrix = projectionMatrix;
}



Matrix4 AdvancedRenderer::GetModelMatrix(const Vector3& scalingFactor, const EulerAngles& rotationAngles, const Vector3& translationDisplacement)
{
	Matrix4 modelMatrix = Matrix4::IdentityMatrix4();

	Matrix4 scalingMatrix = GetScalingMatrix(scalingFactor);
	Matrix4 rotationMatrix = GetRotationMatrix(rotationAngles);
	Matrix4 translationMatrix = GetTranslationMatrix(translationDisplacement);

	modelMatrix = scalingMatrix * rotationMatrix * translationMatrix;

	return modelMatrix;
}



Matrix4 AdvancedRenderer::GetViewMatrix(const Vector3& viewTranslation, const EulerAngles& viewRotation)
{
	Matrix4 viewMatrix = Matrix4::IdentityMatrix4();

	Matrix4 inverseTranslationMatrix = GetTranslationMatrix(viewTranslation.GetNegatedVector3());
	Matrix4 inverseRotationMatrix = GetRotationMatrix(viewRotation).GetTranspose();

	viewMatrix = inverseTranslationMatrix * inverseRotationMatrix;

	return viewMatrix;
}



Matrix4 AdvancedRenderer::GetOrthographicProjectionMatrix(const Vector2& bottomLeft, const Vector2& topRight, float nearDistance /*= -1.0f*/, float farDistance /*= 1.0f*/)
{
	Matrix4 projectionMatrix = Matrix4::IdentityMatrix4();

	float inverseHorizontal = 1.0f / (topRight.X - bottomLeft.X);
	float inverseVertical = 1.0f / (topRight.Y - bottomLeft.Y);
	float inverseDepth = 1.0f / (farDistance - nearDistance);

	projectionMatrix.m_Matrix4[0] = 2.0f * inverseHorizontal;
	projectionMatrix.m_Matrix4[5] = 2.0f * inverseVertical;
	projectionMatrix.m_Matrix4[10] = 2.0f * inverseDepth;

	projectionMatrix.m_Matrix4[12] = -(topRight.X + bottomLeft.X) * inverseHorizontal;
	projectionMatrix.m_Matrix4[13] = -(topRight.Y + bottomLeft.Y) * inverseVertical;
	projectionMatrix.m_Matrix4[14] = -(farDistance + nearDistance) * inverseDepth;

	return projectionMatrix;
}



Matrix4 AdvancedRenderer::GetPerspectiveProjectionMatrix(float fovAngleInDegrees, float aspectRatio, float nearDistance, float farDistance)
{
	Matrix4 projectionMatrix = Matrix4::IdentityMatrix4();

	float inverseDepth = 1.0f / (farDistance - nearDistance);
	float fieldOfView = 1.0f / TangentOfDegrees(fovAngleInDegrees / 2.0f);

	float projectionWidth = (aspectRatio > 1.0f) ? (fieldOfView / aspectRatio) : fieldOfView;
	float projectionHeight = (aspectRatio > 1.0f) ? fieldOfView : (fieldOfView * aspectRatio);

	projectionMatrix.m_Matrix4[0] = 0.0f;
	projectionMatrix.m_Matrix4[2] = (farDistance + nearDistance) * inverseDepth;
	projectionMatrix.m_Matrix4[3] = 1.0f;
	projectionMatrix.m_Matrix4[4] = -projectionWidth;
	projectionMatrix.m_Matrix4[5] = 0.0f;
	projectionMatrix.m_Matrix4[9] = projectionHeight;
	projectionMatrix.m_Matrix4[10] = 0.0f;
	projectionMatrix.m_Matrix4[14] = -2.0f * nearDistance * farDistance * inverseDepth;
	projectionMatrix.m_Matrix4[15] = 0.0f;

	return projectionMatrix;
}



Matrix4 AdvancedRenderer::GetScalingMatrix(const Vector3& scalingFactor)
{
	Matrix4 scalingMatrix = Matrix4::IdentityMatrix4();

	scalingMatrix.m_Matrix4[0] *= scalingFactor.X;
	scalingMatrix.m_Matrix4[5] *= scalingFactor.Y;
	scalingMatrix.m_Matrix4[10] *= scalingFactor.Z;

	return scalingMatrix;
}



Matrix4 AdvancedRenderer::GetRotationMatrix(const EulerAngles& rotationAngles)
{
	Matrix4 rotationMatrix = Matrix4::IdentityMatrix4();

	float cosRollAngle = CosineOfDegrees(rotationAngles.m_RollAngleInDegrees);
	float sinRollAngle = SineOfDegrees(rotationAngles.m_RollAngleInDegrees);

	float cosPitchAngle = CosineOfDegrees(rotationAngles.m_PitchAngleInDegrees);
	float sinPitchAngle = SineOfDegrees(rotationAngles.m_PitchAngleInDegrees);

	float cosYawAngle = CosineOfDegrees(rotationAngles.m_YawAngleInDegrees);
	float sinYawAngle = SineOfDegrees(rotationAngles.m_YawAngleInDegrees);

	rotationMatrix.m_Matrix4[0] = cosPitchAngle * cosYawAngle;
	rotationMatrix.m_Matrix4[1] = cosPitchAngle * sinYawAngle;
	rotationMatrix.m_Matrix4[2] = -sinPitchAngle;

	rotationMatrix.m_Matrix4[4] = (-cosRollAngle * sinYawAngle) + (cosYawAngle * sinPitchAngle * sinRollAngle);
	rotationMatrix.m_Matrix4[5] = (cosYawAngle * cosRollAngle) + (sinPitchAngle * sinYawAngle * sinRollAngle);
	rotationMatrix.m_Matrix4[6] = cosPitchAngle * sinRollAngle;

	rotationMatrix.m_Matrix4[8] = (sinYawAngle * sinRollAngle) + (cosYawAngle * cosRollAngle * sinPitchAngle);
	rotationMatrix.m_Matrix4[9] = (-cosYawAngle * sinRollAngle) + (cosRollAngle * sinPitchAngle * sinYawAngle);
	rotationMatrix.m_Matrix4[10] = cosPitchAngle * cosRollAngle;

	return rotationMatrix;
}



Matrix4 AdvancedRenderer::GetTranslationMatrix(const Vector3& translationDisplacement)
{
	Matrix4 translationMatrix = Matrix4::IdentityMatrix4();

	translationMatrix.m_Matrix4[12] = translationDisplacement.X;
	translationMatrix.m_Matrix4[13] = translationDisplacement.Y;
	translationMatrix.m_Matrix4[14] = translationDisplacement.Z;

	return translationMatrix;
}



Matrix4 AdvancedRenderer::GetLocalBasis()
{
	Matrix4 localBasis = Matrix4::IdentityMatrix4();

	localBasis.m_Matrix4[0] = 0.0f;
	localBasis.m_Matrix4[1] = -1.0f;
	localBasis.m_Matrix4[2] = 0.0f;
	localBasis.m_Matrix4[3] = 0.0f;

	localBasis.m_Matrix4[4] = 0.0f;
	localBasis.m_Matrix4[5] = 0.0f;
	localBasis.m_Matrix4[6] = 1.0f;
	localBasis.m_Matrix4[7] = 0.0f;

	localBasis.m_Matrix4[8] = 1.0f;
	localBasis.m_Matrix4[9] = 0.0f;
	localBasis.m_Matrix4[10] = 0.0f;
	localBasis.m_Matrix4[11] = 0.0f;

	localBasis.m_Matrix4[12] = 0.0f;
	localBasis.m_Matrix4[13] = 0.0f;
	localBasis.m_Matrix4[14] = 0.0f;
	localBasis.m_Matrix4[15] = 1.0f;

	return localBasis;
}



void AdvancedRenderer::DrawMeshWithVAO(Mesh* drawableMesh, Material* meshMaterial /*= nullptr*/)
{
	Material* renderMaterial = (meshMaterial == nullptr) ? m_DefaultMaterial : meshMaterial;
	
	m_MeshRenderer->SetMesh(drawableMesh);
	m_MeshRenderer->SetMaterial(renderMaterial);
	m_MeshRenderer->RenderMeshWithMaterial(m_ModelMatrix, m_ViewMatrix, m_ProjectionMatrix);
}



void AdvancedRenderer::DrawPointsMesh(Mesh* pointsMesh, size_t numberOfVertices, size_t numberOfIndices, float pointSize /*= 1.0f*/)
{
	pointsMesh->m_RenderInstructions.clear();
	pointsMesh->AddRenderInstruction(numberOfVertices, numberOfIndices, POINTS_PRIMITIVE);
	
	SetPointSize(pointSize);
	DrawMeshWithVAO(pointsMesh);
}



void AdvancedRenderer::DrawLinesMesh(Mesh* linesMesh, size_t numberOfVertices, size_t numberOfIndices, float lineThickness /*= 1.0f*/)
{
	linesMesh->m_RenderInstructions.clear();
	linesMesh->AddRenderInstruction(numberOfVertices, numberOfIndices, LINES_PRIMITIVE);
	
	SetLineWidth(lineThickness);
	DrawMeshWithVAO(linesMesh);
}



void AdvancedRenderer::DrawLineLoopMesh(Mesh* lineLoopMesh, size_t numberOfVertices, size_t numberOfIndices, float lineThickness /*= 1.0f*/)
{
	lineLoopMesh->m_RenderInstructions.clear();
	lineLoopMesh->AddRenderInstruction(numberOfVertices, numberOfIndices, LINE_LOOP_PRIMITIVE);
	
	SetLineWidth(lineThickness);
	DrawMeshWithVAO(lineLoopMesh);
}



void AdvancedRenderer::DrawPolygonMesh(Mesh* polygonMesh, size_t numberOfVertices, size_t numberOfIndices, Material* meshMaterial /*= nullptr*/)
{
	polygonMesh->m_RenderInstructions.clear();
	polygonMesh->AddRenderInstruction(numberOfVertices, numberOfIndices, TRIANGLES_PRIMITIVE);
	
	DrawMeshWithVAO(polygonMesh, meshMaterial);
}



void AdvancedRenderer::TriangulatePolygon(size_t numberOfVertices, std::vector<uint32_t>& polygonIndices)
{
	for (uint32_t vertexIndex = 2; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		polygonIndices.push_back(0);
		polygonIndices.push_back(vertexIndex - 1);
		polygonIndices.push_back(vertexIndex);
	}
}



void AdvancedRenderer::Draw2DMonospacedText(const Vector2& startingLetterMinimums, const char* asciiText, float cellHeight, float cellAspectRatio, const MonospaceFont* font /*= nullptr*/, const RGBA& textTint /*= RGBA::WHITE*/)
{
	Mesh textMesh;
	Material textMaterial = *m_DefaultMaterial;
	textMaterial.SetDiffuseTexture(font->GetFontTexture());

	std::vector<Vertex3D> textVertices;
	std::vector<uint32_t> textIndices;

	Vertex3D textVertex;
	textVertex.m_Color = textTint;
	Vector3 letterBoundingPoint = Vector3::ZERO;

	float cellWidth = cellHeight * cellAspectRatio;
	Vector2 cellDimensions(cellWidth, cellHeight);
	Vector2 letterMinimums = startingLetterMinimums;

	size_t textLength = strlen(asciiText);
	for (size_t glyphIndex = 0; glyphIndex < textLength; ++glyphIndex)
	{
		char asciiGlyph = asciiText[glyphIndex];
		if (asciiGlyph == '\n')
		{
			letterMinimums.X = startingLetterMinimums.X;
			letterMinimums.Y -= cellHeight;
			continue;
		}

		uint32_t letterStartingIndex = textVertices.size();

		AABB2 glyphTextureCoordinates = font->GetTextureCoordsForGlyph(asciiGlyph);
		Vector2 textureMinimums = glyphTextureCoordinates.minimums;
		Vector2 textureMaximums = glyphTextureCoordinates.maximums;

		Vector2 letterMaximums = letterMinimums + cellDimensions;

		letterBoundingPoint.X = letterMinimums.X;
		letterBoundingPoint.Y = letterMinimums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMaximums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMaximums.X;
		letterBoundingPoint.Y = letterMinimums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMaximums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMaximums.X;
		letterBoundingPoint.Y = letterMaximums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMinimums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMinimums.X;
		letterBoundingPoint.Y = letterMaximums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMinimums.Y);
		textVertices.push_back(textVertex);

		textIndices.push_back(letterStartingIndex + 0);
		textIndices.push_back(letterStartingIndex + 1);
		textIndices.push_back(letterStartingIndex + 2);
		textIndices.push_back(letterStartingIndex + 0);
		textIndices.push_back(letterStartingIndex + 2);
		textIndices.push_back(letterStartingIndex + 3);

		letterMinimums.X += cellWidth;
	}

	size_t numberOfVertices = textVertices.size();
	size_t numberOfIndices = textIndices.size();

	textMesh.WriteToMesh(&textVertices[0], &textIndices[0], numberOfVertices, numberOfIndices);
	DrawPolygonMesh(&textMesh, numberOfVertices, numberOfIndices, &textMaterial);
}



void AdvancedRenderer::Draw2DProportionalText(const Vector2& startingLetterMinimums, const char* asciiText, float cellHeight, const ProportionalFont* font /*= nullptr*/, const RGBA& textTint /*= RGBA::WHITE*/)
{
	Mesh textMesh;
	Material textMaterial = *m_DefaultMaterial;

	std::vector<Vertex3D> textVertices;
	std::vector<uint32_t> textIndices;

	Vertex3D textVertex;
	textVertex.m_Color = textTint;
	Vector3 letterBoundingPoint = Vector3::ZERO;
	
	Vector2 cursorPosition = startingLetterMinimums;
	cursorPosition.Y += cellHeight;

	float textScale = font->GetCharacterScaleForCellHeight(cellHeight);

	ProportionalGlyph* previousGlyph = nullptr;

	size_t textLength = strlen(asciiText);
	for (size_t glyphIndex = 0; glyphIndex < textLength; ++glyphIndex)
	{
		char asciiGlyph = asciiText[glyphIndex];
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

		Vector2 letterMinimums;
		Vector2 letterMaximums;

		letterMinimums.X = cursorPosition.X + (glyphOffset.X * textScale);
		letterMaximums.Y = cursorPosition.Y - (glyphOffset.Y * textScale);

		letterMaximums.X = letterMinimums.X + (glyphSize.X * textScale);
		letterMinimums.Y = letterMaximums.Y - (glyphSize.Y * textScale);

		textMaterial.SetDiffuseTexture(font->GetFontTextureForGlyph(*currentGlyph));
		AABB2 glyphTextureBounds = font->GetTextureBoundsForGlyph(*currentGlyph);
		Vector2 textureMinimums = glyphTextureBounds.minimums;
		Vector2 textureMaximums = glyphTextureBounds.maximums;

		uint32_t letterStartingIndex = textVertices.size();

		letterBoundingPoint.X = letterMinimums.X;
		letterBoundingPoint.Y = letterMinimums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMaximums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMaximums.X;
		letterBoundingPoint.Y = letterMinimums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMaximums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMaximums.X;
		letterBoundingPoint.Y = letterMaximums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMinimums.Y);
		textVertices.push_back(textVertex);

		letterBoundingPoint.X = letterMinimums.X;
		letterBoundingPoint.Y = letterMaximums.Y;
		textVertex.m_Position = letterBoundingPoint;
		textVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMinimums.Y);
		textVertices.push_back(textVertex);

		textIndices.push_back(letterStartingIndex + 0);
		textIndices.push_back(letterStartingIndex + 1);
		textIndices.push_back(letterStartingIndex + 2);
		textIndices.push_back(letterStartingIndex + 0);
		textIndices.push_back(letterStartingIndex + 2);
		textIndices.push_back(letterStartingIndex + 3);

		cursorPosition.X += currentGlyph->GetAdvance().X * textScale;
		previousGlyph = currentGlyph;
	}

	size_t numberOfVertices = textVertices.size();
	size_t numberOfIndices = textIndices.size();

	textMesh.WriteToMesh(&textVertices[0], &textIndices[0], numberOfVertices, numberOfIndices);
	DrawPolygonMesh(&textMesh, numberOfVertices, numberOfIndices, &textMaterial);
}



void AdvancedRenderer::BindFrameBuffer(FrameBuffer* frameBuffer, size_t colorTargetIndex /*= 0U*/)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetFrameBufferID());
	SetProjectionViewport(IntVector2::ZERO, frameBuffer->GetFrameDimensions());

	uint32_t renderTarget = GL_COLOR_ATTACHMENT0 + colorTargetIndex;
	glDrawBuffers(1, &renderTarget);
}



void AdvancedRenderer::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}



void AdvancedRenderer::CopyFrameBufferToBackBuffer(FrameBuffer* frameBuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer->GetFrameBufferID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

	int readWidth = frameBuffer->GetFrameDimensions().X;
	int readHeight = frameBuffer->GetFrameDimensions().Y;

	int drawWidth = m_WindowDimensions.X;
	int drawHeight = m_WindowDimensions.Y;

	glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, drawWidth, drawHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}