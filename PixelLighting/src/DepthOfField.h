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
	void RenderCircleOfConfusion(Renderer& renderer, float znear, float zfar);
	void BlurCoc(Renderer& renderer);

	Texture* m_ColorTex, * m_DepthTex, * m_CocTex, * m_BluredCocTex;
	FrameBuffer* m_CocFbo, * m_BluredCocFbo;
	Shader *m_FullscreenQuadShader, *m_CocShader, *m_BlurShader, *m_TexToScreenShader;

public:
	int m_ScreenWidth, m_ScreenHeight, m_CocWidth, m_CocHeight;
	FrameBuffer *m_FullscreenFbo;
	bool m_Active;
	
	float m_Aperture, m_FocusPlane, m_FocalLength;

	DepthOfField(int scrWidth, int scrHeight, int cocWidth=0, int cocHeight=0);
	~DepthOfField();

	void Apply(Renderer& renderer, float znear, float zfar);
	void CompileFullscreenQuadShader(const std::string& filename);
	void CompileCocShader(const std::string& filename);
	void CompileBlurShader(const std::string& filename);
	void CompileTexToScreenShader(const std::string& filename);

	// TODO: since now image is rendered to a texture of fixed size, resizing window might break everything
};