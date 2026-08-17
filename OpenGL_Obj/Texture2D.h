#pragma once
#include<string>
#include<Glad/glad.h>
class Texture2D
{
public:
	Texture2D(const std::string fileName);
	Texture2D(int width, int height);
	Texture2D(int width, int height,GLenum rgb_type);

	Texture2D(int width, int height,const void* Texdata);
	~Texture2D();
	void Bind(unsigned int slot);
	void UnBind();
	void SubImage2D(int offsetX,int offsetY,int width,int height,const void* Texdata);
	inline unsigned int GetID() { return m_RendererID; }
	inline int GetWidth() { return m_Width; }
	inline int GetHeight() { return m_Height; }
private:
	int m_Width;
	int m_Height;
	int m_BBP;
	unsigned int m_RendererID;
	unsigned char* m_TexData;
};

