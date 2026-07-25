#pragma once
#include "Core.h"
#include <vector>
class VertexBuffer;
class IndexBuffer;
class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	void AddVertexBuffer(const Ref(VertexBuffer)& vb);
	void SetIndexBuffer(const Ref(IndexBuffer)& ib);

	void Bind();
	void UnBind();
	//Getter
	const std::vector<Ref(VertexBuffer)>& GetVertexBuffers() const { return m_VertexBuffers; }
	const Ref(IndexBuffer)& GetIndexBuffer() const { return m_IndexBuffer; }
private:

	unsigned int m_ShaderPipeID;
	unsigned int m_RendererID;
	std::vector<Ref(VertexBuffer)> m_VertexBuffers;
	Ref(IndexBuffer) m_IndexBuffer;
};