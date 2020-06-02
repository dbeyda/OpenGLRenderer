#include "Light.h"

Light::Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec4 color)
	:m_Type(type), m_Position(position), m_Direction(direction), m_Color(color)
{}

Light::~Light()
{
}

glm::mat4 Light::GetViewProjection(float fovy, float aspect, float near, float far)
{
	glm::mat4 lightProjection = glm::perspective(fovy, aspect, near, far);
	glm::mat4 lightView = glm::lookAt(m_Position, m_Position + m_Direction, glm::vec3(0, 1, 0));
	return lightProjection * lightView;
}

void Light::SetRGB(int r, int g, int b, int a)
{
	float red = (float)r / 255.0;
	float green = (float)g / 255.0;
	float blue = (float)b / 255.0;
	float alpha = (float)a / 255.0;
	
	m_Color = glm::vec4(red, green, blue, alpha);
}
