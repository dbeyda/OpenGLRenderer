#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Obj.h"
#include "Renderer.h"
#include "OpenGLHelper.h"

#include "glm/glm.hpp"

class Model
{
private:
	std::string m_Label;
	unsigned int m_AmbientSlot, m_DiffuseSlot, m_BumpSlot;
	
	Renderer& m_Renderer;
	VertexArray *m_Va;
	VertexBuffer *m_Vb;
	VertexBufferLayout *m_Layout;
	IndexBuffer* m_Ib;
	Texture *m_AmbientTexture, *m_DiffuseTexture, *m_BumpTexture;

public:
	int m_HasAmbientTexture, m_HasDiffuseTexture,
		m_HasBumpTexture;

	float m_Mshi, m_Ka, m_Kd, m_Ks;
	glm::mat4 m_Model;

	Model(const std::string&, const std::string& path, Renderer& renderer);
	~Model();
	void Bind(Shader& shader);
	void Draw(Shader& shader);
	void Unbind();
	std::string GetLabel();
};