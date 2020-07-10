#include <iostream>
#include "FrameBuffer.h"
#include "OpenGLHelper.h"


FrameBuffer::FrameBuffer(unsigned int target, unsigned int width, unsigned int height)
	:m_RendererID(0), m_Target(target), m_RenderWidth(width), m_RenderHeight(height)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::FrameBuffer(unsigned int target)
	: m_RendererID(0), m_Target(target), m_RenderWidth(0), m_RenderHeight(0)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::~FrameBuffer()
{
	Unbind();
	GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::AttachTexture(unsigned int attachment, Texture &texture)
{
	Bind();
	GLCall(glFramebufferTexture2D(m_Target, attachment, texture.m_Target, texture.GetRendererID(), 0));
	Unbind();
}

void FrameBuffer::Bind()
{
	GLCall(glBindFramebuffer(m_Target, m_RendererID));
}

bool FrameBuffer::Check()
{
	Bind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[OpenGL Error]: FrameBuffer not complete." << std::endl;
		Unbind();
		return false;
	}
	Unbind();
	return true;
}

void FrameBuffer::Unbind()
{
	GLCall(glBindFramebuffer(m_Target, 0));
}
