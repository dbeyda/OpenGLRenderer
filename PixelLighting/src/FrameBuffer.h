#pragma once

#include <GL/glew.h>
#include "Texture.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID, m_Target;
public:
	unsigned int m_RenderWidth, m_RenderHeight;

	FrameBuffer(unsigned int TARGET);
	FrameBuffer(unsigned int TARGET, unsigned int width, unsigned int height);
	~FrameBuffer();

	void AttachTexture(unsigned int attachment, Texture &texture);
	void Bind();
	void Unbind();
	bool Check();
};
