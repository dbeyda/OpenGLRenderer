#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

#include "Shader.h"
#include "FrameBuffer.h"
#include "OpenGLHelper.h"

class Renderer
{
private:
    FrameBuffer *m_FullscreenFbo;
    VertexBuffer *m_FullscreenVB;
    VertexArray *m_FullscreenVAO;
    IndexBuffer *m_FullscreenIB;
    VertexBufferLayout *m_FullscreenVBL;

    void initFullscreenQuad();

public:
    FrameBuffer* m_TargetFbo;
    int m_DefaultViewportWidth, m_DefaultViewportHeight;

    Renderer();
    Renderer(int width, int height);
    ~Renderer();
    void Clear() const;
    void Clear(unsigned int bufferEnum);
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void DrawFullscreenQuad(const Shader& shader) const;
    void SetViewport(int width, int height);
    void SetDrawBuffer(unsigned int drawBuffer);
    void SetRenderTarget(FrameBuffer* fbo, int vwWidth, int vwHeight, int drawBuffer = GL_COLOR_ATTACHMENT0);
    void SetRenderTarget(FrameBuffer* fbo, int drawBuffer = GL_COLOR_ATTACHMENT0);
    void ResetRenderTarget();
    void UpdateDefaultViewport(int width, int height);
};