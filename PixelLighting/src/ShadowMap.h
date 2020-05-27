#pragma once

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

#include "Texture.h"
#include "FrameBuffer.h"
#include "Renderer.h"

class ShadowMap
{
private:
	int m_Width, m_Height;
	unsigned int m_Slot, m_Type;
	Texture* m_Texture;
	FrameBuffer* m_Fbo;


public:
	std::vector<std::pair<float, float>> m_Offsets;

	ShadowMap(unsigned int slot);
	~ShadowMap();

	void LoadShadowTexture(int width, int height, unsigned int type);
	void GenerateCircularOffsets(int nSamples, bool jittered = true);
	void GenerateSquareOffsets(int nSamples, bool jittered = true);
	
	void BindForReading();
	void BindForReading(unsigned int slot);
	void UnbindForReading();
	
	void SetAsRenderTarget(Renderer& renderer);
	void ResetAsRenderTarget();
};