#include "ShadowMap.h"

ShadowMap::ShadowMap(unsigned int slot)
	:m_Width(0), m_Height(0), m_Slot(slot),
	m_Texture(nullptr), m_Fbo(nullptr), m_Type(0)
{
	m_Fbo = new FrameBuffer(GL_FRAMEBUFFER);
}

ShadowMap::~ShadowMap()
{
	if (m_Texture) delete m_Texture;
	if (m_Fbo) delete m_Fbo;
}

void ShadowMap::LoadShadowTexture(int width, int height, unsigned int type)
{
	m_Width = width;
	m_Height = height;
	m_Type = type;
	m_Texture = new Texture(m_Slot);
	m_Texture->LoadEmpty(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_Width, m_Height, GL_DEPTH_COMPONENT, m_Type);
	
	m_Texture->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_Texture->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_Texture->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_Texture->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_Texture->SetTexParameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	m_Texture->SetTexParameteri(GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	m_Fbo->AttachTexture(GL_DEPTH_ATTACHMENT, *m_Texture);
	if (!m_Fbo->Check())
	{
		std::cout << "ShadowMap FBO is not complete. Exiting..." << std::endl << std::endl;
		exit(1);
	}
}

void ShadowMap::GenerateCircularOffsets(int nSamples, bool jittered /*= true*/)
{
	m_Offsets.clear();
	float jitR = 0.0, jitTeta = 0.0;

	for (int r = 0; r < nSamples / 4; r++)
		for (float teta = 0.0; teta < 1.0; teta += 0.25)
		{
			if (jittered)
			{
				jitR = (float)std::rand() / RAND_MAX;
				jitTeta = 0.25 * (float)std::rand() / RAND_MAX;
			}

			float x = sqrt(r + jitR) * glm::cos(2 * glm::pi<float>() * (teta + jitTeta));
			float y = sqrt(r + jitR) * glm::sin(2 * glm::pi<float>() * (teta + jitTeta));
			m_Offsets.push_back(std::make_pair(x, y));
		}
}

void ShadowMap::GenerateSquareOffsets(int nSamples, bool jittered /*= true*/)
{
	m_Offsets.clear();
	float jitR = 0.0, jitTeta = 0.0;
	int limit = std::sqrt(nSamples);

	for (int i = -limit / 2; i <= limit / 2; i++)
		for (int j = -limit / 2; j <= limit / 2; j++)
		{
			if (jittered)
			{
				jitR = (float)std::rand() / RAND_MAX;
				jitTeta = 0.25 * (float)std::rand() / RAND_MAX;
			}

			float jitX = -0.5 + (float)std::rand() / RAND_MAX;
			float jitY = -0.5 + (float)std::rand() / RAND_MAX;
			m_Offsets.push_back(std::make_pair(i + jitX, j + jitY));
		}
}

void ShadowMap::BindForReading()
{
	m_Texture->Bind();
}

void ShadowMap::BindForReading(unsigned int slot)
{
	m_Texture->Bind(slot);
}

void ShadowMap::UnbindForReading()
{
	m_Texture->Unbind();
}

void ShadowMap::SetAsRenderTarget(Renderer& renderer)
{
	m_Fbo->Bind();
	renderer.SetViewport(m_Width, m_Height);
	renderer.SetDrawBuffer(GL_NONE);
}

void ShadowMap::ResetAsRenderTarget()
{
	m_Fbo->Unbind();
}

