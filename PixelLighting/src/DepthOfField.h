#pragma once

#include "FrameBuffer.h"

class DepthOfField
{
private:

public:
	int m_ScreenWidth, m_ScreenHeight, m_CocWidth, m_CocHeight;
	Texture *m_ColorTex, *m_DepthTex, *m_CocTex;
	FrameBuffer *m_Fbo;

	DepthOfField(int scrWidth, int scrHeight, int cocWidth, int cocHeight);
	~DepthOfField();
};