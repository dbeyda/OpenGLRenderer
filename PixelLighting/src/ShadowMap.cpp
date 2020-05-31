#include "ShadowMap.h"

ShadowMap::ShadowMap(unsigned int slot)
	:m_Width(0), m_Height(0), m_ShadowMapSlot(slot),
	m_Texture(nullptr), m_Fbo(nullptr), m_Type(0),
	m_JitTexture(nullptr), m_JitTextureSlot(0),
	m_Blur(0.2)
{
	m_Fbo = new FrameBuffer(GL_FRAMEBUFFER);
}

ShadowMap::~ShadowMap()
{
	if (m_Texture) delete m_Texture;
	if (m_JitTexture) delete m_JitTexture;
	if (m_Fbo) delete m_Fbo;
}

void ShadowMap::LoadShadowTexture(int width, int height, unsigned int type)
{
	m_Width = width;
	m_Height = height;
	m_Type = type;
	m_Texture = new Texture(m_ShadowMapSlot);
	m_Texture->TexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_Width, m_Height, GL_DEPTH_COMPONENT, m_Type);
	
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

void ShadowMap::LoadJitTexture(int samplesPixel, int width, int height, unsigned int slot, void *data)
{
	m_JitTextureSlot = slot;
	m_JitTexture = new Texture(m_JitTextureSlot);

	m_JitTexture->TexImage3D(GL_TEXTURE_3D, GL_RG8, width, height, samplesPixel, GL_RG, GL_UNSIGNED_BYTE, data);
	m_JitTexture->SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_JitTexture->SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_JitTexture->SetTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_JitTexture->SetTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	m_JitTexture->SetTexParameteri(GL_TEXTURE_WRAP_R, GL_REPEAT);
}


void ShadowMap::SampleCircularOffset(std::vector<unsigned char>& v, int n, float rMin, float deltaR, float tetaMin, float deltaTeta, bool jittered)
{
	float jitR = 0.0, jitTeta = 0.0;
	while(n--)
		{
			if (jittered)
			{
				jitR = deltaR * (float)std::rand() / (float)RAND_MAX;
				jitTeta = deltaTeta * (float)std::rand() / (float)RAND_MAX;
			}

			float x = sqrt(rMin + jitR) * glm::cos(2 * glm::pi<float>() * (tetaMin + jitTeta));
			float y = sqrt(rMin + jitR) * glm::sin(2 * glm::pi<float>() * (tetaMin + jitTeta));

			// transforming from [-1, 1] to [0, 255]
			unsigned char xByte = (unsigned char) round(255 * ((x + 1.0) / 2.0));
			unsigned char yByte = (unsigned char) round(255 * ((y + 1.0) / 2.0));
			v.push_back(xByte);
			v.push_back(yByte);
		}
}

void ShadowMap::GenerateJitTexture(int samplesPerPixel, int width, int height, int radius, unsigned int slot)
{
	// allocate 3D vector of RGBA bytes
	std::vector<unsigned char> jitData;
	float rInc = 1.0 / (float) radius;
	float tetaInc = (float) radius / (float) samplesPerPixel;
	for (float r = 0.0; r < 1.0; r += rInc)
		for (float teta = 0.0; teta < 1.0; teta += tetaInc)
			SampleCircularOffset(jitData, width * height, r, rInc, teta, tetaInc, true);
	
	LoadJitTexture(samplesPerPixel, width, height, slot, jitData.data());
}


void ShadowMap::BindForReading()
{
	if (m_Texture) m_Texture->Bind();
	if (m_JitTexture) m_JitTexture->Bind();
}

void ShadowMap::BindForReading(unsigned int slot, unsigned int jitSlot)
{
	if (m_Texture) m_Texture->Bind(slot);
	if (m_JitTexture) m_JitTexture->Bind(jitSlot);
}

void ShadowMap::UnbindForReading()
{
	if (m_Texture) m_Texture->Unbind();
	if (m_JitTexture) m_JitTexture->Unbind();
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

