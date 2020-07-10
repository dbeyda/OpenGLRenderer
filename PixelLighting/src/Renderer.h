#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "OpenGLHelper.h"

class Renderer
{
public:
    FrameBuffer* m_TargetFbo;
    int m_DefaultViewportWidth, m_DefaultViewportHeight;

    Renderer();
    Renderer(int width, int height);
    void Clear() const;
    void Clear(unsigned int bufferEnum);
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void SetViewport(int width, int height);
    void SetDrawBuffer(unsigned int drawBuffer);
    void SetRenderTarget(FrameBuffer* fbo, int vwWidth, int vwHeight, int drawBuffer = GL_COLOR_ATTACHMENT0);
    void SetRenderTarget(FrameBuffer* fbo, int drawBuffer = GL_COLOR_ATTACHMENT0);
    void ResetRenderTarget();
    void UpdateDefaultViewport(int width, int height);
};