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
	unsigned int m_Target;
	Texture(unsigned int slot);
	~Texture();

	void LoadFromFile(const std::string& path);
	void LoadEmpty(unsigned int TARGET, int internalFormat, int width, int height,
				   unsigned int format, unsigned int type);
	void Bind() const;
	void Bind(unsigned int slot);
	void Unbind();
	void SetTexParameteri(unsigned int pName, unsigned int pValue);

	inline int getWidth() const { return m_Width;  }
	inline int GetHight() const { return m_Height;  }
	inline int GetRendererID() const { return m_RendererID; }
};