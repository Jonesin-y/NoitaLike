#include "pch.h"
#include <Glad/glad.h>
#include "Buffer.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
	m_ShaderPipeID = 0;
	glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray()
{
	UnBind();
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddVertexBuffer(const Ref(VertexBuffer)& vb)
{
	Bind();
	vb->Bind();  // 必须先绑定 VBO，再设置顶点属性指针
	BufferLayout layout = vb->GetBufferLayout();
	auto& elements = layout.GetElements();
	for (auto& element : elements)
	{
		glEnableVertexAttribArray(m_ShaderPipeID);
		glVertexAttribPointer(
			m_ShaderPipeID,
			element.count,
			element.type,
			element.normalized,
			layout.GetStride(),
			(const void*)(intptr_t)element.offset
		);
		m_ShaderPipeID++;
	}
	m_VertexBuffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(const Ref(IndexBuffer)& ib)
{
	m_IndexBuffer = ib;
}


void VertexArray::Bind()
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::UnBind()
{
	glBindVertexArray(0);
}
