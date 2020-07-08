#pragma once

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

#include "Texture.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "OpenGLHelper.h"


class ShadowMap
{
private:
	unsigned int m_ShadowMapSlot, m_Type, m_JitTextureSlot;
	Texture* m_Texture;
	Texture* m_JitTexture;

	void SampleCircularOffset(std::vector<unsigned char>& v, int n, float rMin, float deltaR, float tetaMin, float deltaTeta, bool jittered = true);
	void LoadJitTexture(int samplesPerTexel, int width, int height, unsigned int slot, void *data = nullptr);


public:
	int m_Width, m_Height;
	FrameBuffer* m_Fbo;
	float m_Blur;
	std::vector<std::pair<float, float>> m_Offsets;

	ShadowMap(unsigned int slot);
	~ShadowMap();

	void LoadShadowTexture(int width, int height, unsigned int type);
	void GenerateJitTexture(int samplesPerPixel, int width, int height, int radius, unsigned int slot);
	
	void BindForReading();
	void BindForReading(unsigned int slot, unsigned int jitSlot=0);
	void UnbindForReading();
};