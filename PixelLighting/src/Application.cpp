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
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lightning
float ambientStrength = 0.03f;
glm::vec4 ambientLightColor(1.0f);

float ka = 0.0;
float kd = 8.0;

float kc = 2;
float kl = 0.01;
float kq = 0.000010;

int sexp = 30;

float mshi = 48.0;
float ks = 5.0;


float sDif = 0.5;
float mDif = 0.5;


int main(void)
{
    std::string objPath = "";
    objPath = "res/models/golfball/golfball.obj";
    objPath = "res/models/formula 1/Formula 1 mesh.obj";
    // objPath = "res/models/stones/stones.obj";

    std::string texPath = "";
    texPath = "res/models/formula 1/Substance SpecGloss/Right ones/formula1_DefaultMaterial_Diffuse.png";
    // texPath = "res/models/stones/stones.jpg";
    unsigned int texFormat = GL_RGB;

    glm::vec3 cameraPos =       glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraFront =     glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp =        glm::vec3(0.0f, 1.0f, 0.0f);
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* setting OpenGL core profile */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* set camera callbacks */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

        Obj obj(objPath);
        obj.Load();

        VertexArray va;
        VertexBuffer vb(obj.positions.data() , obj.positions.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(obj.indices.data(), obj.indices.size());


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
        float increment = 0.3f;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            // per-frame time logic
            // --------------------
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // input
            // -----
            processInput(window);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 rotatedModel = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 1, 0));
            rotatedModel = glm::rotate(rotatedModel, glm::radians(rotation), glm::vec3(0, 1, 0));
            glm::mat4 mvp = projection * view * rotatedModel;
            glm::mat4 mv = view * rotatedModel;
            glm::mat4 invTransMvp = glm::inverseTranspose(mvp);

            glm::mat4 invTransMv = glm::inverseTranspose(view * rotatedModel);

            /* Render here */
            renderer.Clear();
            
            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);
            shader.SetUniformMat4f("u_MV", mv);
            shader.SetUniformMat4f("u_proj", projection);
            shader.SetUniformMat4f("u_invTransMV", invTransMv);

            shader.SetUniform1f("u_ambientStrength", ambientStrength);
            shader.SetUniform4f("u_ambientLightColor", ambientLightColor.r, ambientLightColor.g, ambientLightColor.b, 1.0f);
            shader.SetUniform1f("u_ka", ka);
            shader.SetUniform1f("u_kd", kd);
            shader.SetUniform1f("u_kc", kc);
            shader.SetUniform1f("u_kl", kl);
            shader.SetUniform1f("u_kq", kq);
            shader.SetUniform1i("u_sexp", sexp);
            shader.SetUniform1f("u_mshi", mshi);
            shader.SetUniform1f("u_ks", ks);

            renderer.Draw(va, ib, shader);

            rotation += increment;

            ImGui_ImplGlfwGL3_NewFrame();
            {
                ImGui::Begin("Camera");
                ImGui::Text("FPS (%.1f FPS)", ImGui::GetIO().Framerate);
                ImGui::Text("Camera position");                           // Display some text (you can use a format string too)
                ImGui::SliderFloat3("x y z", glm::value_ptr(camera.Position), -5000.0f, 5000.0f, nullptr, 5.0f);
                if (ImGui::Button("Reset Camera"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                    camera.Position = glm::vec3(0);
                ImGui::End();

                ImGui::Begin("Lighting");
                ImGui::SliderFloat("Ambient Light Intensity", &ambientStrength, 0.0f, 1.0f);
                ImGui::ColorEdit3("Ambient Light Color", glm::value_ptr(ambientLightColor)); // Edit 3 floats representing a color
                ImGui::InputFloat("ka", &ka, 0.1, 0.5);
                ImGui::InputFloat("kd", &kd, 0.1, 0.5);
                ImGui::Text("Light Source");
                ImGui::InputFloat("kc", &kc, 0.2, 0.5);
                ImGui::InputFloat("kl", &kl, 0.05, 0.1);
                ImGui::InputFloat("kq", &kq, 0.005, 0.01);
                ImGui::SliderInt("sexp", &sexp, 0, 128);
                ImGui::SliderFloat("mshi", &mshi, 0, 600);
                ImGui::SliderFloat("ks", &ks, 0, 10);

                ImGui::End();
/*
                ImGui::Text("mDif");
                ImGui::SameLine();
                ImGui::SliderFloat("", &mDif, 0, 1);
                ImGui::Text("sDif");
                ImGui::SameLine();
                ImGui::SliderFloat("", &sDif, 0, 1);
  */
  }
            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// ----------------------------------------------------------------------------------------------------------

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool running = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        running = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, running);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, running);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, running);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, running);
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}