#include "pch.h"
#include "Buffer.h"
#include"pch.h"
#include<Glad/glad.h>
#include "Buffer.h"
BufferElement::BufferElement(ShaderDataType dataType, const std::string& dataName, bool Normalized)
	:count(ShaderDataTypeToCount(dataType)), size(ShaderDataTypeToSize(dataType)), type(ShaderDataTypeToGladType(dataType)), normalized(Normalized)
{

}
BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) :
	m_Elements(elements)
{
	CaculateLayout();
}
void BufferLayout::CaculateLayout()
{
	unsigned int t_offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.offset = t_offset;
		m_Stride += element.size;
		t_offset += element.size;
	}
}
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	UnBind();
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer(const void* data, unsigned int count)
	:m_Count(count)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	UnBind();
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
RenderBuffer::RenderBuffer(int width, int height, int samples, GLenum internalFormat)
{
	glGenRenderbuffers(1, &m_RendererID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	if (samples == 0)
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	else
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
}

RenderBuffer::~RenderBuffer()
{
	UnBind();
	glDeleteRenderbuffers(1, &m_RendererID);
}

void RenderBuffer::Bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}

void RenderBuffer::UnBind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

FrameBuffer::~FrameBuffer()
{
	UnBind();
	glDeleteFramebuffers(1, &m_RendererID);
}

void FrameBuffer::Bind(GLenum bufferType)
{
	m_bufferType = bufferType;
	glBindFramebuffer(m_bufferType, m_RendererID);
}

void FrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AddTexture2D(unsigned int textureID)
{
	Bind(m_bufferType);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("帧缓冲区加载纹理失败!\n");
}

void FrameBuffer::AddRendererBuffer(unsigned int bufferID)
{
	Bind(m_bufferType);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, bufferID);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("帧缓冲区加载渲染缓冲区失败!\n");
}

