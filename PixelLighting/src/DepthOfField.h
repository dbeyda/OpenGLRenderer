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
	void InitEffect();
	void RenderCircleOfConfusion(Renderer& renderer);
public:
	int m_ScreenWidth, m_ScreenHeight, m_CocWidth, m_CocHeight;
	Texture *m_ColorTex, *m_DepthTex, *m_CocTex;
	FrameBuffer *m_CocFbo, *m_FullscreenFbo;
	Shader *m_FullscreenQuadShader, *m_CocShader;

	DepthOfField(int scrWidth, int scrHeight, int cocWidth=0, int cocHeight=0);
	~DepthOfField();

	void Apply(Renderer& renderer);
	void CompileFullscreenQuadShader(const std::string& filename);
	void CompileCocShader(const std::string& filename);

	// TODO: since now image is rendered to a texture of fixed size, resizing window might break everything
};