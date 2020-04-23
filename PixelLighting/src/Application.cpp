#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Obj.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


int main(void)
{
    std::string objPath = "res/models/stones/stones.obj";
    std::string texPath = "res/models/stones/stones.jpg";
    unsigned int texFormat = GL_RGB;

    Obj obj(objPath);
    obj.Load();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*
    Using OpenGL Compatibility Profile
    TheCherno already changed to OpenGL Core Profile on video "Vertex Arrays in OpenGL"
    */

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    if (glewInit() != GLEW_OK) {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    {   
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(obj.positions.data() , obj.positions.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(obj.indices.data(), obj.indices.size());

        glm::mat4 proj = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 5000.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),  // eye
                           glm::vec3(0.0f, 0.0f, 0.0f), // center
                           glm::vec3(0.0f, 1.0f, 0.0f)   // up
        );
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0));
        //model = glm::scale(model, glm::vec3(10.0f));

        Texture texture(texPath, texFormat);
        texture.Bind();

        Shader shader("res/shaders/ObjLoader.shader");

        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer;

        float rotation = 0.0f;
        float increment = 0.5f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glm::mat4 rotatedModel = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
            rotatedModel = glm::rotate(rotatedModel, glm::radians(rotation), glm::vec3(0, 1, 1));
            glm::mat4 mvp = proj * view * rotatedModel;

            /* Render here */
            renderer.Clear();
            
            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);

            renderer.Draw(va, ib, shader);

            rotation += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}