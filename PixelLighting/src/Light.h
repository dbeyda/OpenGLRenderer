#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"


class Light
{
private:
	
public:
	enum class LightType{ Directional, Point, Spotlight };

	LightType m_Type;
	glm::vec3 m_Position;
	glm::vec3 m_Direction;
	glm::vec4 m_Color;

	float m_spotExponent;

	Light(LightType type, glm::vec3 position, glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0), glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0));
	~Light();
	glm::mat4 GetViewProjection(float fovy, float aspect, float near, float far);
	void SetRGB(int r, int g, int b, int a = 255);
};