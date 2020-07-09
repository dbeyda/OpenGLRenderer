#include "DepthOfField.h"
#include <iostream>

DepthOfField::DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight)
	: m_ScreenWidth(scrWidth), m_ScreenHeight(scrHeight), m_CocWidth(cocWidth), m_CocHeight(cocHeight),
	m_FullscreenQuadShader(nullptr), m_FullscreenVB(nullptr), m_FullscreenVAO(nullptr), m_FullscreenIB(nullptr),
	m_FullscreenVBL(nullptr)
{
	InitOffscreenRendering();
	InitFullscreenQuad();
}

DepthOfField::~DepthOfField()
{
	delete m_ColorTex;
	delete m_FullscreenFbo;
	delete m_DepthTex;
	delete m_CocTex;
	delete m_FullscreenQuadShader;
	delete m_FullscreenVAO;
	delete m_FullscreenVB;
	delete m_FullscreenVBL;
	delete m_FullscreenIB;
}

void DepthOfField::InitOffscreenRendering()
{
	m_ColorTex = new Texture(1);
	m_ColorTex->TexImage2D(GL_TEXTURE_2D, GL_RGBA, m_CocWidth, m_CocHeight, GL_RGBA, GL_FLOAT);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_ColorTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_DepthTex = new Texture(2);
	m_DepthTex->TexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_CocWidth, m_CocHeight, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_DepthTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_CocTex = new Texture(3);
	m_CocTex->TexImage2D(GL_TEXTURE_2D, GL_RED, m_CocWidth, m_CocHeight, GL_RED, GL_FLOAT);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_CocTex->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_FullscreenFbo = new FrameBuffer(GL_FRAMEBUFFER);
	m_FullscreenFbo->AttachTexture(GL_COLOR_ATTACHMENT0, *m_ColorTex);
	m_FullscreenFbo->AttachTexture(GL_DEPTH_ATTACHMENT, *m_DepthTex);

	if (!m_FullscreenFbo->Check())
	{
		std::cout << "DepthOfField Screen FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}
}

void DepthOfField::InitFullscreenQuad()
{
	const float fullscreenPoints[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f,  5.0f, 0.0f,
	 5.0f, -1.0f, 0.0f
	};
	const unsigned int indices[] = {
	0, 1, 2
	};

	m_FullscreenVAO = new VertexArray();
	m_FullscreenVB = new VertexBuffer(fullscreenPoints, 9 * sizeof(float));
	m_FullscreenVBL = new VertexBufferLayout();
	m_FullscreenVBL->Push<float>(3); // position
	m_FullscreenVAO->AddBuffer(*m_FullscreenVB, *m_FullscreenVBL);
	m_FullscreenIB = new IndexBuffer(indices, 3);
}

void DepthOfField::CreateFullscreenQuadShader(const std::string& filename)
{
	m_FullscreenQuadShader = new Shader(filename);
}

void DepthOfField::Apply(Renderer& renderer, float width, float height)
{
	// preparing textures
	m_ColorTex->Bind();
	m_DepthTex->Bind();
	// preparing shader
	m_FullscreenQuadShader->Bind();
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneColor", (signed int)m_ColorTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform1i("samplers.MainSceneDepth", (signed int)m_DepthTex->GetRendererID());
	m_FullscreenQuadShader->SetUniform2f("viewportSize", width, height);
	// draw call
	renderer.Draw(*m_FullscreenVAO, *m_FullscreenIB, *m_FullscreenQuadShader);
}