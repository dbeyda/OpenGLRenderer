
#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
    :m_TargetFbo(nullptr), m_DefaultViewportWidth(1280), m_DefaultViewportHeight(720)
{}

Renderer::Renderer(int width, int height)
    : m_TargetFbo(nullptr), m_DefaultViewportWidth(width), m_DefaultViewportHeight(height)
{}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}


void Renderer::SetViewport(int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void Renderer::SetDrawBuffer(unsigned int drawBuffer)
{
    GLCall(glDrawBuffer(drawBuffer));
}

void Renderer::Clear() const
{
    GLCall(bool enabled = glIsEnabled(GL_DEPTH_TEST));
    if (enabled)
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
    else
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }
}

void Renderer::Clear(unsigned int bufferEnum)
{
    GLCall(glClear(bufferEnum));
}

void Renderer::SetRenderTarget(FrameBuffer* fbo, int vwWidth, int vwHeight, int drawBuffer = GL_COLOR_ATTACHMENT0)
{
    m_TargetFbo = fbo;
    m_TargetFbo->Bind();
    SetViewport(vwWidth, vwHeight);
    SetDrawBuffer(drawBuffer);
}

void Renderer::ResetRenderTarget()
{
    m_TargetFbo->Unbind();
    m_TargetFbo = nullptr;
    SetViewport(m_DefaultViewportWidth, m_DefaultViewportHeight);
    SetDrawBuffer(GL_BACK);
}

void Renderer::UpdateDefaultViewport(int width, int height)
{
    m_DefaultViewportWidth = width;
    m_DefaultViewportHeight = height;
}