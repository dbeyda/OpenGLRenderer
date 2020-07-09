#pragma once

#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "OpenGLHelper.h"
#include "Shader.h"
#include "Renderer.h"

class DepthOfField
{
private:
	void InitOffscreenRendering();
	void InitFullscreenQuad();
public:
	int m_ScreenWidth, m_ScreenHeight, m_CocWidth, m_CocHeight;
	Texture *m_ColorTex, *m_DepthTex, *m_CocTex;
	FrameBuffer *m_FullscreenFbo;
	Shader* m_FullscreenQuadShader;
	VertexBuffer* m_FullscreenVB;
	VertexArray* m_FullscreenVAO;
	IndexBuffer* m_FullscreenIB;
	VertexBufferLayout* m_FullscreenVBL;

	DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight);
	~DepthOfField();

	void Apply(Renderer& renderer, float width, float height);
	void CreateFullscreenQuadShader(const std::string& filename);

	// TODO: since now image is rendered to a texture of fixed size, resizing window might break everything
};