#define SHADOWMAP_SAMPLES 64
#define JITTER_MAP_WIDTH 164
#define JITTER_MAP_HEIGHT 64
#define JITTER_MAP_RADIUS 2

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "ShadowMap.h"
#include "DepthOfField.h"

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
unsigned int SCR_WIDTH = 1440;
unsigned int SCR_HEIGHT = 900;
int SHADOW_WIDTH = 768;
int SHADOW_HEIGHT = 768;
float SHADOW_NEAR = 0.1f;
float SHADOW_FAR = 60.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0, 1.0, 0.0));
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
int sexp = 4;


int main(void)
{
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

	glfwSwapInterval(0);


	if (glewInit() != GLEW_OK) {
		std::cout << "Error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_ONE, GL_ONE));
		
		Renderer renderer(SCR_WIDTH, SCR_HEIGHT);
		DepthOfField dof(SCR_WIDTH, SCR_HEIGHT, SCR_WIDTH / 16, SCR_HEIGHT / 16);
		dof.CompileFullscreenQuadShader("res/shaders/DoF/quad.shader");
		dof.CompileCocShader("res/shaders/DoF/CircleOfConfusion.shader");

		// ----------- Models
		std::string golfballPath = "res/models/golfball/golfball.obj";
		Model golfball("Golfball", golfballPath, renderer);
		golfball.m_Model = glm::translate(golfball.m_Model, glm::vec3(0, -8, 0));
		
		std::string stonesPath = "res/models/stones/stones.obj";
		Model stones("Stones", stonesPath, renderer);
		stones.m_Model = glm::translate(stones.m_Model, glm::vec3(-10, -10, -10));
		stones.m_Model = glm::scale(stones.m_Model, glm::vec3(20, 20, 20));

		Model stonesLeft("Stones", stonesPath, renderer);
		stonesLeft.m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(-10, -10, 10));
		stonesLeft.m_Model = glm::scale(stonesLeft.m_Model, glm::vec3(20, 20, 20));
		stonesLeft.m_Model = glm::rotate(stonesLeft.m_Model, glm::radians(90.0f), glm::vec3(0, 1, 0));

		Model stonesRight("Stones", stonesPath, renderer);
		stonesRight.m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(10, -10, -10));
		stonesRight.m_Model = glm::scale(stonesRight.m_Model, glm::vec3(20, 20, 20));
		stonesRight.m_Model = glm::rotate(stonesRight.m_Model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

		Model stonesBottom("Stones", stonesPath, renderer);
		stonesBottom.m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(-10, -10, 10));
		stonesBottom.m_Model = glm::scale(stonesBottom.m_Model, glm::vec3(20, 20, 20));
		stonesBottom.m_Model = glm::rotate(stonesBottom.m_Model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		stonesBottom.m_HasAmbientTexture = 0;
		stonesBottom.m_HasDiffuseTexture = 0;
		stonesBottom.m_HasBumpTexture = 0;

		std::vector<Model*> models({ &golfball, &stones, &stonesLeft, &stonesBottom, &stonesRight});
		
		// ----------- Lights

		glm::vec3 spotLightDir = glm::normalize(glm::vec3(-1.5, -1, -1.5));
		glm::vec3 spotLightPos = glm::vec3(10, 0.5, 10);
		
		Light spotlight1(Light::LightType::Spotlight, glm::vec3(9, -3, 8), glm::normalize(glm::vec3(-1, -0.5, -0.9)), 33.0);
		Light spotlight2(Light::LightType::Spotlight, glm::vec3(-9, -3, 8), glm::normalize(glm::vec3(1, -0.5, -0.9)), 33.0);
		Light spotlight3(Light::LightType::Spotlight, glm::vec3(0, -3, 8), glm::normalize(glm::vec3(0, -1, -2)), 33.0);

		spotlight1.SetRGB(0, 255, 0);
		spotlight2.SetRGB(0, 0, 255);
		spotlight3.SetRGB(255, 0, 0);

		std::vector<Light*> lights({ &spotlight1, &spotlight2, &spotlight3 });
		// ----------- Shaders
		Shader shader("res/shaders/PhongShadow.shader");
		shader.Bind();

		Shader zShader("res/shaders/ShadowMapGen.shader");

		ShadowMap shadowMap(3);
		shadowMap.LoadShadowTexture(SHADOW_WIDTH, SHADOW_HEIGHT, GL_FLOAT);
		shadowMap.GenerateJitTexture(SHADOWMAP_SAMPLES, JITTER_MAP_WIDTH, JITTER_MAP_HEIGHT, JITTER_MAP_RADIUS, 4);

		float rotation = 0.0f;
		float increment = 0.3f;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{

			processInput(window);
			renderer.UpdateDefaultViewport(SCR_WIDTH, SCR_HEIGHT);

			renderer.SetRenderTarget(dof.m_FullscreenFbo);
			renderer.Clear();

			// per-frame time logic
			// --------------------
			glm::mat4 golfballRotatedModel = glm::rotate(golfball.m_Model, glm::radians(rotation), glm::vec3(1, 0, 0));

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			for (Light* l : lights)
			{
				// ------- 1st Pass: shadow map
				renderer.SetRenderTarget(shadowMap.m_Fbo, GL_NONE);
				renderer.Clear();
				zShader.Bind();

				glm::mat4 lightVp = l->GetViewProjection(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, SHADOW_NEAR, SHADOW_FAR);
				
				for (Model* m : models)
				{
					// Draw each model
					glm::mat4 model;
					if (m->GetLabel() == "Golfball")
						model = golfballRotatedModel;
					else
						model = m->m_Model;
					glm::mat4 lightMvp = lightVp * model;
					zShader.SetUniformMat4f("u_MVP", lightMvp);
					m->Draw(zShader);
				}


				// --------- 2nd Pass: lighting
				
				//renderer.ResetRenderTarget();
				renderer.SetRenderTarget(dof.m_FullscreenFbo);
				renderer.Clear(GL_DEPTH_BUFFER_BIT);
				shader.Bind();

				glm::mat4 cameraProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
				glm::mat4 cameraView = camera.GetViewMatrix();
				glm::mat4 invTransCameraView = glm::inverseTranspose(cameraView);

				// Light
				glm::vec3 cameraLightPos = cameraView * glm::vec4(l->m_Position, 1.0f);
				glm::vec3 cameraSpotLightDir = invTransCameraView * glm::vec4(l->m_Direction, 1.0f);
				cameraSpotLightDir = glm::normalize(cameraSpotLightDir);

				// Shadow
				glm::mat4 biasMatrix(
					0.5, 0.0, 0.0, 0.0,
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0
				);
				glm::mat4 depthBiasMVP = biasMatrix * lightVp;

				shadowMap.BindForReading(3, 4);
				shader.SetUniform1f("shadowMap.blur", shadowMap.m_Blur);
				shader.SetUniform1f("shadowMap.samplesCount", (float)SHADOWMAP_SAMPLES);
				shader.SetUniform1f("shadowMap.jitterMapWidth", (float)JITTER_MAP_WIDTH);
				shader.SetUniform1f("shadowMap.jitterMapHeight", (float)JITTER_MAP_HEIGHT);
				shader.SetUniform1f("shadowMap.jitterRadius", (float)JITTER_MAP_RADIUS);
				shader.SetUniform1i("shadowMap.DepthMap", 3);
				shader.SetUniform1i("shadowMap.JitOffsets", 4);
				shader.SetUniform4f("u_globalLightColor", globalLightColor.r, globalLightColor.g, globalLightColor.b, 1.0f);
				shader.SetUniform1f("u_globalLightStrength", globalLightStrength);
				shader.SetUniform3f("light.cameraSpacePos", cameraLightPos.x, cameraLightPos.y, cameraLightPos.z);
				shader.SetUniform3f("light.cameraSpaceDir", cameraSpotLightDir.x, cameraSpotLightDir.y, cameraSpotLightDir.z);
				shader.SetUniform1f("light.kc", kc);
				shader.SetUniform1f("light.kl", kl);
				shader.SetUniform1f("light.kq", kq);
				shader.SetUniform1i("light.sexp", l->m_SpotExponent);
				shader.SetUniform3f("light.color", l->m_Color.r, l->m_Color.g, l->m_Color.b);

				for (Model* m : models)
				{
					// Draw each model
					glm::mat4 model;
					if (m->GetLabel() == "Golfball")
						model = golfballRotatedModel;
					else
						model = m->m_Model;
					glm::mat4 mvp = cameraProjection * cameraView * model;
					glm::mat4 mv = cameraView * model;
					glm::mat4 invTransMv = glm::inverseTranspose(cameraView * model);
					shader.SetUniformMat4f("u_MVP", mvp);
					shader.SetUniformMat4f("u_MV", mv);
					shader.SetUniformMat4f("u_invTransMV", invTransMv);

					m->Bind(shader);
					if (m->m_HasBumpTexture)
						shader.SetUniform1i("material.hasBumpTexture", useBumpTexture);
					shader.SetUniformMat4f("u_DepthMvp", depthBiasMVP * model);

					m->Draw(shader);
					m->Unbind();
				}
			}

			// ---------------- POST PROCESSING FX

			// Depth Of Field
			dof.Apply(renderer);

			


			// ---------------- User Interface
			rotation += increment;

			ImGui_ImplGlfwGL3_NewFrame();
			{
				ImGui::Begin("Camera");
				ImGui::Text("FPS (%.1f FPS)", ImGui::GetIO().Framerate);
				ImGui::Text("Camera position");
				ImGui::SliderFloat3("x y z", glm::value_ptr(camera.Position), -5000.0f, 5000.0f, nullptr, 5.0f);
				if (ImGui::Button("Reset Camera"))
					camera.Position = glm::vec3(0);
				ImGui::End();

				ImGui::Begin("Light");
				ImGui::SliderFloat("Ambient Light Intensity", &globalLightStrength, 0.0f, 1.0f);
				ImGui::ColorEdit3("Ambient Light Color", glm::value_ptr(globalLightColor));

				ImGui::Text("\nAll Lights:");
				ImGui::InputFloat("kc", &kc, 0.2, 0.5);
				ImGui::InputFloat("kl", &kl, 0.05, 0.1);
				ImGui::InputFloat("kq", &kq, 0.005, 0.01);
				
				int i = 0;
				for (Light* l : lights)
				{
					ImGui::Text("\nLight %d:", i);
					ImGui::SliderFloat(("sexp " + std::to_string(i)).c_str(), &(l->m_SpotExponent), 0, 128);
					ImGui::ColorEdit3(("Light " + std::to_string(i) + " Color").c_str(), glm::value_ptr(l->m_Color));
					i++;
				}

				ImGui::Text("\nOther:");
				ImGui::SliderFloat("Shadows Blur", &shadowMap.m_Blur, 0.0, 1.0);
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

