
#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << std::endl;
        std::cout << "[OpenGL Error] (" << error << "): "
                  << glewGetErrorString(error) << std::endl;
        std::cout << "> Func: " << function <<
            std::endl << "> File: " << file << ":" << line << '\n' << std::endl;
        return false;
    }
    return true;
}

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
