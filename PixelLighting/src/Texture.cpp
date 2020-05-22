#include <iostream>
#include "Texture.h"
#include "stb_image/stb_image.h"

std::string getFileExtension(std::string& filepath);


Texture::Texture()
	: m_RendererID(0), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0), m_Slot(0),
	m_Target(0)
{}

Texture::~Texture() 
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::LoadFromFile(const std::string& path, unsigned int slot)
{
	m_FilePath = path;
	m_Slot = slot;
	m_Target = GL_TEXTURE_2D;

	unsigned int format;
	int n_channels;

	std::string ext = getFileExtension(m_FilePath);
	if (ext == "jpg" || ext == "jpeg")
	{
		format = GL_RGB;
		n_channels = 3;
	}
	else if (ext == "png")
	{
		format = GL_RGBA;
		n_channels = 4;
	}
	else
	{
		std::cout << "[Texture Error] File extension " << ext << "not recognized." << std::endl;
		exit(1);
	}

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, n_channels);

	GLCall(glGenTextures(1, &m_RendererID));
	Bind();

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

void Texture::LoadEmpty(unsigned int target, int internalFormat, int width, int height,
						unsigned int format, unsigned int type)
{
	m_Target = target;
	GLCall(glGenTextures(1, &m_RendererID));
	Bind();
	GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, NULL));
	Unbind();
}

void Texture::Bind() const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + m_Slot));
	GLCall(glBindTexture(m_Target, m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(m_Target, m_RendererID));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(m_Target, 0));
}

std::string getFileExtension(std::string& filepath)
{
	return filepath.substr(filepath.find_last_of(".") + 1);
}
