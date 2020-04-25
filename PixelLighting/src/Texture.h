#pragma once

#include "Renderer.h"

class Texture 
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	unsigned int m_Slot;
public:
	Texture();
	~Texture();

	void Load(const std::string& path, unsigned int slot = 0);
	void Bind() const;
	void Unbind();

	inline int getWidth() const { return m_Width;  }
	inline int GetHight() const { return m_Height;  }
};