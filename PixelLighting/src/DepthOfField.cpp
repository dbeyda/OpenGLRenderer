#include "DepthOfField.h"
#include <iostream>

/*
https://www.photomodeler.com/kb/what_is_camera_focal_length/#:~:text=A%2035mm%20film%20camera%20has,a%20focal%20length%20of%2050mm
A 35mm film camera has a negative that is about 36mm wide by 24mm high 
(the “35” comes from the physical width of the film stock that is exactly 35mm wide).
A 'normal lens' (has a field of view that appears 'natural' to humans) on a 35mm film camera
has a focal length of 50mm.
*/

DepthOfField::DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight)
	: m_ScreenWidth(scrWidth), m_ScreenHeight(scrHeight), m_CocWidth(cocWidth), m_CocHeight(cocHeight),
	m_FullscreenQuadShader(nullptr), m_FullscreenFbo(nullptr), m_CocFbo(nullptr), m_CocShader(nullptr),
	m_Aperture(1.4f), m_FocusPlane(12.0f), m_FocalLength(1.4f), m_BleedingMult(30.0f), m_BleedingBias(0.02f),
	m_BluredCocFbo(nullptr), m_BluredCocTex(nullptr), m_BlurShader(nullptr)
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
	delete m_CocShader;
	delete m_FullscreenQuadShader;
	delete m_CocFbo;
	delete m_BluredCocFbo;
	delete m_BluredCocTex;
	delete m_BlurShader;
}

void DepthOfField::InitEffect()
{
	m_ColorTex = new Texture(1);
	m_ColorTex->TexImage2D(GL_TEXTURE_2D, GL_RGBA, m_ScreenWidth, m_ScreenHeight, GL_RGBA, GL_FLOAT);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_DepthTex = new Texture(2);
	m_DepthTex->TexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ScreenWidth, m_ScreenHeight, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
	m_CocTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_CocTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	m_CocFbo = new FrameBuffer(GL_FRAMEBUFFER, m_CocWidth, m_CocHeight);
	m_CocFbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_CocTex);

	if (!m_CocFbo->Check())
	{
		std::cout << "CircleOfConfusion FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}

	m_BluredCocTex = new Texture(4);
	m_BluredCocTex->TexImage2D(GL_TEXTURE_2D, GL_RED, m_CocWidth, m_CocHeight, GL_RED, GL_FLOAT);
	m_BluredCocTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_BluredCocTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_BluredCocTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_BluredCocTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_BluredCocFbo = new FrameBuffer(GL_FRAMEBUFFER, m_CocWidth, m_CocHeight);
	m_BluredCocFbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_BluredCocTex);

	if (!m_BluredCocFbo->Check())
	{
		std::cout << "BluredCoc FBO is not complete. Exiting..." << std::endl << std::endl;
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

void DepthOfField::CompileBlurShader(const std::string& filename)
{
	m_BlurShader = new Shader(filename);
}

void DepthOfField::RenderCircleOfConfusion(Renderer& renderer, float znear, float zfar)
{
	renderer.SetRenderTarget(m_CocFbo);
	renderer.Clear();
	
	m_DepthTex->Bind();
	m_ColorTex->Bind();
	m_CocShader->Bind();

	m_CocShader->SetUniform1i("samplers.MainSceneDepth", (signed int)m_DepthTex->GetRendererID());
	m_CocShader->SetUniform1f("aperture", m_Aperture);
	m_CocShader->SetUniform1f("focusPlane", m_FocusPlane);
	m_CocShader->SetUniform1f("focalLength", m_FocalLength);
	m_CocShader->SetUniform1f("znear", znear);
	m_CocShader->SetUniform1f("zfar", zfar);

	m_CocShader->SetUniform2f("viewportSize", (float)m_CocFbo->m_RenderWidth, (float)m_CocFbo->m_RenderHeight);

	renderer.DrawFullscreenQuad(*m_CocShader);
}

void DepthOfField::BlurCoc(Renderer& renderer)
{
	renderer.SetRenderTarget(m_BluredCocFbo);
	renderer.Clear();

	m_CocTex->Bind();
	
	m_BlurShader->Bind();
	m_BlurShader->SetUniform1i("samplers.OriginalImage", (signed int)m_CocTex->GetRendererID());
	m_BlurShader->SetUniform1f("angle", 90.0f);
	m_BlurShader->SetUniform1i("numSamples", 15);
	m_BlurShader->SetUniform1f("radius", 0.01f);
	m_BlurShader->SetUniform2f("viewportSize", (float)m_BluredCocFbo->m_RenderWidth, (float)m_BluredCocFbo->m_RenderHeight);

	renderer.DrawFullscreenQuad(*m_BlurShader);
}

void DepthOfField::Apply(Renderer& renderer, float znear, float zfar)
{
	RenderCircleOfConfusion(renderer, znear, zfar);
	//BlurCoc(renderer);

	renderer.ResetRenderTarget();
	renderer.Clear();
	
	// preparing textures
	//m_BluredCocTex->Bind();
	m_CocTex->Bind();

	m_ColorTex->Bind();
	m_DepthTex->Bind();
	// preparing shader
	m_FullscreenQuadShader->Bind();

	//m_FullscreenQuadShader->SetUniform1i("samplers.Coc", (signed int)m_BluredCocTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1i("samplers.Coc", (signed int)m_CocTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneColor", (signed int)m_ColorTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneDepth", (signed int)m_DepthTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1f("focalPlane", m_FocusPlane);
	m_FullscreenQuadShader->SetUniform2f("viewportSize", (float) renderer.m_DefaultViewportWidth, (float) renderer.m_DefaultViewportHeight);
	// TODO: fix this
	m_FullscreenQuadShader->SetUniform1f("bleedingMult", m_BleedingMult); //m_BleedingMult);
	m_FullscreenQuadShader->SetUniform1f("bleedingBias", m_BleedingBias); //m_BleedingBias);
	// draw call
	renderer.DrawFullscreenQuad(*m_FullscreenQuadShader);
}