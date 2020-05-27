#include "Model.h"
#include "Obj.h"

Model::Model(const std::string& label, const std::string& path, Renderer& renderer)
	:m_HasAmbientTexture(0), m_HasDiffuseTexture(0),
	m_HasBumpTexture(0), m_AmbientSlot(0), m_DiffuseSlot(1),
	m_BumpSlot(2), m_Renderer(renderer), m_Model(glm::mat4(1.0f)),
	m_Va(nullptr), m_Vb(nullptr), m_Ib(nullptr), m_Layout(nullptr),
	m_Label(label)

{
	Obj obj(path);
	obj.Load();

	m_Va = new VertexArray();
	m_Vb = new VertexBuffer(obj.positions.data(), obj.positions.size() * sizeof(float));
	m_Layout = new VertexBufferLayout();
	m_Layout->Push<float>(3); // position
	m_Layout->Push<float>(3); // normal vector
	m_Layout->Push<float>(2); // texture uv
	m_Layout->Push<float>(3); // tangent vector
	m_Va->AddBuffer(*m_Vb, *m_Layout);

	m_Ib = new IndexBuffer(obj.indices.data(), obj.indices.size());

	// textures
	if (obj.materials.size() && obj.materials[0].ambient_texname.size())
	{
		m_AmbientTexture = new Texture(m_AmbientSlot);
		m_AmbientTexture->LoadFromFile(obj.materials[0].ambient_texname);
		m_HasAmbientTexture = 1;
		// m_AmbientTexture->Bind();
	}

	if (obj.materials.size() && obj.materials[0].diffuse_texname.size())
	{
		m_DiffuseTexture = new Texture(m_DiffuseSlot);
		m_DiffuseTexture->LoadFromFile(obj.materials[0].diffuse_texname);
		m_HasDiffuseTexture = 1;
		// m_DiffuseTexture->Bind(); // really necessary?
	}

	if (obj.materials.size() && obj.materials[0].bump_texname.size());
	{
		m_BumpTexture = new Texture(m_BumpSlot);
		m_BumpTexture->LoadFromFile(obj.materials[0].bump_texname);
		m_HasBumpTexture = 1;
		// m_BumpTexture->Bind();
	}
	if (obj.materials.size())
	{
		tinyobj::material_t& mat = obj.materials[0];
		m_Mshi = mat.shininess;
		m_Ka = mat.ambient[0];
		m_Kd = mat.diffuse[0];
		m_Ks = mat.specular[0];
	}
	m_Va->Unbind();
	m_Vb->Unbind();
	m_Ib->Unbind();
}

Model::~Model()
{
	delete m_Va;
	delete m_Vb;
	delete m_Layout;
	delete m_Ib;
	delete m_DiffuseTexture;
	delete m_AmbientTexture;
	delete m_BumpTexture;
}

void Model::Bind(Shader& shader)
{
	shader.Bind();
	shader.SetUniform1f("material.mshi", m_Mshi);
	shader.SetUniform1f("material.ks", m_Ks);
	shader.SetUniform1f("material.ka", m_Ka);
	shader.SetUniform1f("material.kd", m_Kd);

	shader.SetUniform1i("material.hasAmbientTexture", m_HasAmbientTexture);
	shader.SetUniform1i("material.hasDiffuseTexture", m_HasDiffuseTexture);
	shader.SetUniform1i("material.hasBumpTexture", m_HasBumpTexture);

	shader.SetUniform1i("samplers.AmbientTexture", (signed int) m_AmbientSlot);
	shader.SetUniform1i("samplers.DiffuseTexture", (signed int) m_DiffuseSlot);
	shader.SetUniform1i("samplers.BumpTexture", (signed int) m_BumpSlot);

	if (m_HasAmbientTexture)	m_AmbientTexture->Bind();
	if (m_HasDiffuseTexture)	m_DiffuseTexture->Bind();
	if (m_HasBumpTexture)	m_BumpTexture->Bind();
}

void Model::Draw(Shader& shader)
{
	m_Renderer.Draw(*m_Va, *m_Ib, shader);
}

std::string Model::GetLabel()
{
	return m_Label;
}
