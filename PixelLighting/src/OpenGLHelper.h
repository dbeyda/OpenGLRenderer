#pragma once

#include <GL/glew.h>
#include <iostream>


#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line)
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