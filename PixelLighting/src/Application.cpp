#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

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

unsigned int useBumpTexture = 0;

// settings
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 768;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
float globalLightStrength = 0.0f;
glm::vec4 globalLightColor(1.0f);

float kc = 1;
float kl = 0.005;
float kq = 0.0001;
int sexp = 30;


int main(void)
{
	// system("PAUSE");

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* setting OpenGL core profile */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Unreal Engine 2053 secret preview", NULL, NULL);
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
		
		Renderer renderer;
		
		std::string golfballPath = "res/models/golfball/golfball.obj";
		Model golfball("Golfball", golfballPath, renderer);
		golfball.m_Model = glm::translate(golfball.m_Model, glm::vec3(0, -7, 0));
		
		std::string stonesPath = "res/models/stones/stones.obj";
		Model stones("Stones", stonesPath, renderer);
		stones.m_Model = glm::translate(stones.m_Model, glm::vec3(-10, -10, -10));
		stones.m_Model = glm::scale(stones.m_Model, glm::vec3(20, 20, 20));

		Model stonesLeft("Stones", stonesPath, renderer);
		stonesLeft.m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(-10, -10, 10));
		stonesLeft.m_Model = glm::scale(stonesLeft.m_Model, glm::vec3(20, 20, 20));
		stonesLeft.m_Model = glm::rotate(stonesLeft.m_Model, glm::radians(90.0f), glm::vec3(0, 1, 0));

		Model stonesBottom("Stones", stonesPath, renderer);
		stonesBottom.m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(-10, -10, 10));
		stonesBottom.m_Model = glm::scale(stonesBottom.m_Model, glm::vec3(20, 20, 20));
		stonesBottom.m_Model = glm::rotate(stonesBottom.m_Model, glm::radians(-90.0f), glm::vec3(1, 0, 0));


		std::vector<Model*> models({ &golfball, &stones, &stonesLeft, &stonesBottom});
		Shader shader("res/shaders/PhongBumpMap.shader");
		shader.Bind();

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

			// Light
			glm::vec3 spotLightDir = glm::vec3(0, 0, 0);
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), glm::vec3(10, 0.5, 10));
			glm::mat4 lightMv = view * lightModel;
			glm::mat4 invTransMvLight = glm::inverseTranspose(lightMv);

			glm::vec3 mvLightPos = lightMv * glm::vec4(0, 0, 0, 1);
			glm::vec3 mvSpotLightDir = invTransMvLight * glm::vec4(-1.5, -1, -1.5, 1);

			/* Render here */
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_globalLightColor", globalLightColor.r, globalLightColor.g, globalLightColor.b, 1.0f);
			shader.SetUniform1f("u_globalLightStrength", globalLightStrength);
			shader.SetUniform3f("u_mvLightPos", mvLightPos.x, mvLightPos.y, mvLightPos.z);
			shader.SetUniform3f("u_mvSpotLightDir", mvSpotLightDir.x, mvSpotLightDir.y, mvSpotLightDir.z);
			shader.SetUniform1f("u_kc", kc);
			shader.SetUniform1f("u_kl", kl);
			shader.SetUniform1f("u_kq", kq);
			shader.SetUniform1i("u_sexp", sexp);

			for (Model *m : models)
			{
				// Draw each model
				glm::mat4 model;
				if (m->GetLabel() == "Golfball")
					model = glm::rotate(m->m_Model, glm::radians(rotation), glm::vec3(1, 0, 0));
				else
					model = m->m_Model;
				glm::mat4 mvp = projection * view * model;
				glm::mat4 mv = view * model;
				glm::mat4 invTransMv = glm::inverseTranspose(view * model);

				shader.SetUniformMat4f("u_MVP", mvp);
				shader.SetUniformMat4f("u_MV", mv);
				shader.SetUniformMat4f("u_invTransMV", invTransMv);

				m->Bind(shader);
				shader.SetUniform1i("u_hasBumpTexture", useBumpTexture);
				m->Draw(shader);
			}

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

				ImGui::Begin("Light");
				ImGui::SliderFloat("Ambient Light Intensity", &globalLightStrength, 0.0f, 1.0f);
				ImGui::ColorEdit3("Ambient Light Color", glm::value_ptr(globalLightColor)); // Edit 3 floats representing a color

				ImGui::Text("\nLight Source:");
				ImGui::InputFloat("kc", &kc, 0.2, 0.5);
				ImGui::InputFloat("kl", &kl, 0.05, 0.1);
				ImGui::InputFloat("kq", &kq, 0.005, 0.01);
				ImGui::SliderInt("sexp", &sexp, 0, 128);
				
				ImGui::Text("\nOther:");
				ImGui::SliderFloat("rotationSpeed", &increment, 0, 10);
				
				ImGui::End();

				for (Model* m : models)
				{
					ImGui::Begin(m->GetLabel().c_str());
					ImGui::InputFloat("mshi", &(m->m_Mshi), 0.5, 10);
					ImGui::SliderFloat("ks", &(m->m_Ks), 0, 10);
					ImGui::SliderFloat("kd", &(m->m_Kd), 0, 10);
					ImGui::SliderFloat("ka", &(m->m_Ka), 0, 10);
					ImGui::End();
				}
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
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		useBumpTexture = 0;
	else
		useBumpTexture = 1;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_HEIGHT = (unsigned int)height;
	SCR_WIDTH = (unsigned int)width;
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

/* ----------------------- auxiliary functions -------------------------*/

