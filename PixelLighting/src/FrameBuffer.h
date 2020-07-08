#pragma once

#include <GL/glew.h>
#include "Texture.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID, m_Target;
public:
	FrameBuffer(unsigned int TARGET);
	~FrameBuffer();

	void AttachTexture(unsigned int attachment, Texture &texture);
	void Bind();
	void Unbind();
	bool Check();
};
