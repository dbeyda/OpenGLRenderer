#include "DepthOfField.h"
#include <iostream>

DepthOfField::DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight)
	: m_ScreenWidth(scrWidth), m_ScreenHeight(scrHeight), m_CocWidth(cocWidth), m_CocHeight(cocHeight),
	m_FullscreenQuadShader(nullptr), m_FullscreenFbo(nullptr), m_CocFbo(nullptr), m_CocShader(nullptr)
{
	if (!m_CocWidth) m_CocWidth = scrWidth;
	if (!m_CocHeight) m_CocHeight = scrHeight;
	InitEffect();
}

DepthOfField::~DepthOfField()
{
	delete m_ColorTex;
	delete m_FullscreenFbo;
	delete m_DepthTex;
	delete m_CocTex;
	delete m_FullscreenQuadShader;
	delete m_CocFbo;
}

void DepthOfField::InitEffect()
{
	m_ColorTex = new Texture(1);
	m_ColorTex->TexImage2D(GL_TEXTURE_2D, GL_RGBA, m_ScreenWidth, m_ScreenHeight, GL_RGBA, GL_FLOAT);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_DepthTex = new Texture(2);
	m_DepthTex->TexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ScreenWidth, m_ScreenHeight, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_FullscreenFbo = new FrameBuffer(GL_FRAMEBUFFER, m_ScreenWidth, m_ScreenHeight);
	m_FullscreenFbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_ColorTex);
	m_FullscreenFbo->AttachTexture(GL_DEPTH_ATTACHMENT, *m_DepthTex);

	if (!m_FullscreenFbo->Check())
	{
		std::cout << "DepthOfField Screen FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}

	m_CocTex = new Texture(3);
	m_CocTex->TexImage2D(GL_TEXTURE_2D, GL_RED, m_CocWidth, m_CocHeight, GL_RED, GL_FLOAT);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_CocFbo = new FrameBuffer(GL_FRAMEBUFFER, m_CocWidth, m_CocHeight);
	m_CocFbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_CocTex);

	if (!m_CocFbo->Check())
	{
		std::cout << "CircleOfConfusion FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}
}

void DepthOfField::CompileFullscreenQuadShader(const std::string& filename)
{
	m_FullscreenQuadShader = new Shader(filename);
}

void DepthOfField::CompileCocShader(const std::string& filename)
{
	m_CocShader = new Shader(filename);
}

void DepthOfField::RenderCircleOfConfusion(Renderer& renderer)
{
	renderer.SetRenderTarget(m_CocFbo);
	renderer.Clear(GL_COLOR_BUFFER_BIT);
	
	m_DepthTex->Bind();
	
	m_CocShader->Bind();
	m_CocShader->SetUniform1i("samplers.MainSceneDepth", (signed int)m_DepthTex->GetRendererID());
	m_CocShader->SetUniform2f("viewportSize", (float)m_CocFbo->m_RenderWidth, (float)m_CocFbo->m_RenderHeight);
	
	renderer.DrawFullscreenQuad(*m_CocShader);
}

void DepthOfField::Apply(Renderer& renderer)
{
	RenderCircleOfConfusion(renderer);

	renderer.ResetRenderTarget();
	renderer.Clear();
	// preparing textures
	m_ColorTex->Bind();
	m_DepthTex->Bind();
	// preparing shader
	m_FullscreenQuadShader->Bind();
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneColor", (signed int)m_ColorTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneDepth", (signed int)m_DepthTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform2f("viewportSize", (float) renderer.m_DefaultViewportWidth, (float) renderer.m_DefaultViewportHeight);
	// draw call
	renderer.DrawFullscreenQuad(*m_FullscreenQuadShader);
}