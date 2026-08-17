#pragma once
#include<Glad/glad.h>
#include<string>
#include<vector>
enum ShaderDataType
{
	NONETYPE = 0, Float1, Float2, Float3, Float4, Mat3, Mat4, Int1, Int2, Int3, Int4, Bool
};
inline unsigned int ShaderDataTypeToSize(ShaderDataType dataType)
{
	switch (dataType)
	{
	case NONETYPE:__debugbreak;
	case Float1: return 4;
	case Float2: return 4 * 2;
	case Float3: return 4 * 3;
	case Float4: return 4 * 4;
	case Mat3:	 return 3 * 3 * 4;
	case Mat4:   return 4 * 4 * 4;
	case Int1:   return 4;
	case Int2:   return 4 * 2;
	case Int3:   return 4 * 3;
	case Int4:   return 4 * 4;
	case Bool:   return 1;
	default:return 0;
	}
}
inline unsigned int ShaderDataTypeToCount(ShaderDataType dataType)
{
	switch (dataType)
	{
	case NONETYPE:__debugbreak;
	case Float1: return 1;
	case Float2: return 2;
	case Float3: return 3;
	case Float4: return 4;
	case Mat3:	 return 3 * 3;
	case Mat4:   return 4 * 4;
	case Int1:   return 1;
	case Int2:   return 2;
	case Int3:   return 3;
	case Int4:   return 4;
	case Bool:   return 1;
	default:return 0;

	}
}
inline GLenum ShaderDataTypeToGladType(ShaderDataType dataType)
{
	switch (dataType)
	{
	case NONETYPE:__debugbreak;
	case Float1: return GL_FLOAT;
	case Float2: return GL_FLOAT;
	case Float3: return GL_FLOAT;
	case Float4: return GL_FLOAT;
	case Mat3:	 return GL_FLOAT;
	case Mat4:   return GL_FLOAT;
	case Int1:   return GL_INT;
	case Int2:   return GL_INT;
	case Int3:   return GL_INT;
	case Int4:   return GL_INT;
	case Bool:   return GL_BOOL;
	default:return 0;

	}
}
class BufferElement
{
public:
	unsigned int size;
	unsigned int count;
	GLenum type;
	bool normalized;
	unsigned int offset;
	BufferElement(ShaderDataType dataType, const std::string& dataName, bool Normalized = GL_FALSE);

};
class BufferLayout
{
public:
	BufferLayout() {}
	BufferLayout(std::initializer_list<BufferElement> elements);
	const std::vector<BufferElement>& GetElements() const { return m_Elements; }
	const unsigned int GetStride() const { return m_Stride; }
	void CaculateLayout();
private:
	std::vector<BufferElement> m_Elements;
	unsigned int m_Stride;


};
class VertexBuffer
{
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();
	void SetBufferLayout(const BufferLayout& layout) { m_BufferLayout = layout; }
	const BufferLayout& GetBufferLayout() { return m_BufferLayout; }
	void Bind();
	void UnBind();
private:
	BufferLayout m_BufferLayout;
	unsigned int m_RendererID;

};
class IndexBuffer
{
public:
	IndexBuffer(const void* data, unsigned int count);
	~IndexBuffer();
	void Bind();
	void UnBind();
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
};
class RenderBuffer
{
public:
	RenderBuffer(int width, int height, int samples = 0, GLenum internalFormat = GL_RGBA);
	~RenderBuffer();
	void Bind();
	void UnBind();
	inline unsigned int GetBufferID() { return m_RendererID; }
private:
	unsigned int m_RendererID;
	int m_Width, m_Height;
	int m_Samples;
};
class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();
	void Bind(GLenum bufferType);
	void UnBind();
	void AddTexture2D(unsigned int textureID);
	void AddRendererBuffer(unsigned int bufferID);
private:
	unsigned int m_RendererID;
	GLenum m_bufferType;
};
class UniformBuffer
{
};

