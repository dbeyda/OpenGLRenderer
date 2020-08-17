
#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
    :m_TargetFbo(nullptr), m_DefaultViewportWidth(1280), m_DefaultViewportHeight(720),
    m_FullscreenFbo(nullptr), m_FullscreenVB(nullptr), m_FullscreenVAO(nullptr),
    m_FullscreenIB(nullptr), m_FullscreenVBL(nullptr)
{
    initFullscreenQuad();
}

Renderer::Renderer(int width, int height)
    : m_TargetFbo(nullptr), m_DefaultViewportWidth(width), m_DefaultViewportHeight(height),
    m_FullscreenFbo(nullptr), m_FullscreenVB(nullptr), m_FullscreenVAO(nullptr),
    m_FullscreenIB(nullptr), m_FullscreenVBL(nullptr)
{
    initFullscreenQuad();
}

Renderer::~Renderer()
{
    delete m_FullscreenVAO;
    delete m_FullscreenVB;
    delete m_FullscreenVBL;
    delete m_FullscreenIB;
}

void Renderer::initFullscreenQuad()
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

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawFullscreenQuad(const Shader& shader) const
{
    shader.Bind();
    m_FullscreenVAO->Bind();
    m_FullscreenIB->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_FullscreenIB->GetCount(), GL_UNSIGNED_INT, nullptr));
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

void Renderer::SetRenderTarget(FrameBuffer* fbo, int vwWidth, int vwHeight, int drawBuffer)
{
    m_TargetFbo = fbo;
    m_TargetFbo->Bind();
    SetViewport(vwWidth, vwHeight);
    SetDrawBuffer(drawBuffer);
}

void Renderer::SetRenderTarget(FrameBuffer* fbo, int drawBuffer)
{
    m_TargetFbo = fbo;
    m_TargetFbo->Bind();
    ASSERT(m_TargetFbo->m_RenderWidth && m_TargetFbo->m_RenderHeight);
    SetViewport(m_TargetFbo->m_RenderWidth, m_TargetFbo->m_RenderHeight);
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