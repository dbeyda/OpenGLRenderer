#include "DepthOfField.h"
#include <iostream>

DepthOfField::DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight)
	: m_ScreenWidth(scrWidth), m_ScreenHeight(scrHeight), m_CocWidth(cocWidth), m_CocHeight(cocHeight)
{
	
	m_ColorTex = new Texture(1);
	m_ColorTex->TexImage2D(GL_TEXTURE_2D, GL_RGBA, m_ScreenWidth, m_ScreenHeight, GL_RGBA, GL_FLOAT);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_DepthTex = new Texture(2);
	m_DepthTex->TexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ScreenWidth, m_ScreenHeight, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_CocTex = new Texture(3);
	m_DepthTex->TexImage2D(GL_TEXTURE_2D, GL_R, m_CocWidth, m_CocHeight, GL_COLOR_ATTACHMENT0, GL_FLOAT);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_CocTex->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_CocTex->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_Fbo = new FrameBuffer(GL_FRAMEBUFFER);
	m_Fbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_ColorTex);
	m_Fbo->AttachTexture(GL_DEPTH_ATTACHMENT, *m_DepthTex);


	if (!m_Fbo->Check())
	{
		std::cout << "DepthOfField Screen FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}
}

DepthOfField::~DepthOfField()
{
	delete m_Fbo;
	delete m_ColorTex;
	delete m_DepthTex;
	delete m_CocTex;
}
