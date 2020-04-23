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
    std::string objPath = "";
    objPath = "res/models/golfball/golfball.obj";
    // objPath = "res/models/formula 1/Formula 1 mesh.obj";
    // objPath = "res/models/stones/stones.obj";

    std::string texPath = "";
    // texPath = "res/models/formula 1/Substance SpecGloss/Right ones/formula1_DefaultMaterial_Diffuse.png";
    // texPath = "res/models/stones/stones.jpg";
    unsigned int texFormat = GL_RGB;

    Obj obj(objPath);
    obj.Load();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

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
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),  // eye
                           glm::vec3(0.0f, 0.0f, 0.0f), // center
                           glm::vec3(0.0f, 1.0f, 0.0f)   // up
        );
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0));
        //model = glm::scale(model, glm::vec3(10.0f));

        Texture texture(texPath, texFormat);

        if (texPath.size())
        {
            texture.Load();
            texture.Bind();
        }

        // Shader shader("res/shaders/BasicTexture.shader");
        Shader shader("res/shaders/Phong.shader");


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