#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "Texture2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Buffer.h"
#include "World.h"
#include "MaterialRegister.h"
#include "Chunk.h"
#include "Cell.h"
const int CHUNK_MAX_WIDTH =  256;
const int CHUNK_MAX_HEIGHT = 256;
const int CHUNK_SHIFT = 8;
const int ACTIVE_RECT_LENGTH = 32;
const int ACTIVE_RECT_SHIFT = 5;
World::World(int width, int height)
	:m_Width(width), m_Height(height)
{
	m_Cells.resize(m_Width * m_Height, Cell(AIR));
	
	m_BaseShader = std::make_shared<Shader>("Assets/Shader/Base.vs", "Assets/Shader/Base.fs","BaseShader");
	m_ExtractShader = std::make_shared<Shader>("Assets/Shader/Extract.vs", "Assets/Shader/Extract.fs", "ExtractShader");
	m_BlurShader = std::make_shared<Shader>("Assets/Shader/Blur.vs", "Assets/Shader/Blur.fs", "BlurShader");
	m_CombineShader = std::make_shared<Shader>("Assets/Shader/Combine.vs", "Assets/Shader/Combine.fs", "CombineShader");

	m_srcTexture2D = std::make_shared<Texture2D>(m_Width, m_Height);
	m_MainTexture2D = std::make_shared<Texture2D>(m_Width, m_Height);
	m_ExtractTexture2D = std::make_shared<Texture2D>(m_Width, m_Height);
	m_pingpongTexture2D[0] = std::make_shared<Texture2D>(m_Width, m_Height);
	m_pingpongTexture2D[1] = std::make_shared<Texture2D>(m_Width, m_Height);

	m_MainFBO = std::make_shared<FrameBuffer>();
	m_MainFBO->Bind(GL_FRAMEBUFFER);
	m_MainFBO->AddTexture2D(m_MainTexture2D->GetID());

	m_ExtractFBO = std::make_shared<FrameBuffer>();
	m_ExtractFBO->Bind(GL_FRAMEBUFFER);
	m_ExtractFBO->AddTexture2D(m_ExtractTexture2D->GetID());

	m_pingpongFBO[0] = std::make_shared<FrameBuffer>();
	m_pingpongFBO[0]->Bind(GL_FRAMEBUFFER);
	m_pingpongFBO[0]->AddTexture2D(m_pingpongTexture2D[0]->GetID());
	m_pingpongFBO[1] = std::make_shared<FrameBuffer>();
	m_pingpongFBO[1]->Bind(GL_FRAMEBUFFER);
	m_pingpongFBO[1]->AddTexture2D(m_pingpongTexture2D[1]->GetID());

	m_VAO = std::make_shared<VertexArray>();
	m_srcTexture2D->Bind(0);
	for (int y = 0;y < m_Height >> CHUNK_SHIFT;++y)
	{
		for (int x = 0;x < m_Width >> CHUNK_SHIFT;++x)
		{
			Ref(Chunk) chunk = std::make_shared<Chunk>(x * CHUNK_MAX_WIDTH,y*CHUNK_MAX_HEIGHT,(x+1)*CHUNK_MAX_WIDTH, (y+1)*CHUNK_MAX_HEIGHT, m_Width, m_Height);
			m_Chunks.push_back(chunk);
			if (x % 2 == 0 && y % 2 == 0)
				m_RedChunks.push_back(chunk);
			if(x % 2 == 0 && y % 2 == 1)
				m_BlueChunks.push_back(chunk);
			if (x % 2 == 1 && y % 2 == 0)
				m_YellowChunks.push_back(chunk);
			if (x % 2 == 1 && y % 2 == 1)
				m_GreenChunks.push_back(chunk);

		}
	}
	//std::cout << m_Chunks.size() << std::endl;
	m_ActiveRectCountX = m_Width >> ACTIVE_RECT_SHIFT;
	m_ActiveRectCountY = m_Height >> ACTIVE_RECT_SHIFT;
	m_ActiveRects.resize(m_ActiveRectCountX * m_ActiveRectCountY, 5);
	//m_ActiveRectData.resize(m_ActiveRectCountX * m_ActiveRectCountY, 0);
	m_ChunkCountX = m_Width  / CHUNK_MAX_WIDTH  ;
	m_ChunkCountY = m_Height / CHUNK_MAX_HEIGHT ;
	glm::mat4 model(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(m_Width, m_Height, 0.0f));
	projection = glm::ortho(0.0f, (float)m_Width, (float)m_Height, 0.0f, -1.0f, 1.0f);
	m_BaseShader->SetUniformMat4f("u_model", 1, GL_FALSE, &model[0][0]);
	m_BaseShader->SetUniformMat4f("u_view", 1, GL_FALSE, &view[0][0]);
	m_BaseShader->SetUniformMat4f("u_projection", 1, GL_FALSE, &projection[0][0]);
	float vertices[] =
	{
		0.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,0.0f,
		1.0f,1.0f,1.0f,1.0f
	};
	int indices[] =
	{
		0,1,2,
		1,2,3
	};
	Ref(VertexBuffer) VBO = std::make_shared<VertexBuffer>(vertices, (2 + 2) * 4 * sizeof(float));
	Ref(IndexBuffer)  IBO = std::make_shared<IndexBuffer>(indices, 6);
	BufferLayout layout =
	{
		{Float2,"Position"},
		{Float2,"TexCoord"}
	};
	VBO->SetBufferLayout(layout);
	m_VAO->AddVertexBuffer(VBO);
	m_VAO->SetIndexBuffer(IBO);
}

void World::Update()
{
	static int Toogle = false;
	static int current_frame = 0;
	Toogle = !Toogle;
	current_frame = (current_frame + 1) & 255;

	//static int offset = 0;
	//offset = static_cast<int>(FastRandRange(rand_state, 64)) - 32;
	//UpdateChunkBoundary(offset);

	int leftOrRight = Toogle;
	int startPoint = leftOrRight ? 0 : m_ChunkCountX- 1;
	int endPoint = leftOrRight ? m_ChunkCountX : -1;
	int step = leftOrRight ? 1 : -1;
	for (int y = 0;y < m_ChunkCountY ;++y)
	{
		for (int x = startPoint; x != endPoint; x += step)
		{
			m_Chunks[y * m_ChunkCountX + x]->Update(m_Cells, m_ActiveRects,current_frame);
		}
	}
}

//void World::UpdateChunkBoundary(int offset)
//{
//	m_ChunkCutsX.clear();
//	m_ChunkCutsY.clear();
//	
//	m_ChunkCutsX.push_back(0);
//	for (int i = 1; i < m_ChunkCountX; i++) {
//		if(i==1)
//			m_ChunkCutsX.push_back(i * CHUNK_MAX_WIDTH + offset);
//		else
//			m_ChunkCutsX.push_back(m_ChunkCutsX[i-1] + CHUNK_MAX_WIDTH);
//	}
//	m_ChunkCutsX.push_back(m_Width);
//
//	m_ChunkCutsY.push_back(0);
//	for (int i = 1; i < m_ChunkCountY; i++) {
//		if (i == 1)
//			m_ChunkCutsY.push_back(i * CHUNK_MAX_HEIGHT + offset);
//		else
//			m_ChunkCutsY.push_back(m_ChunkCutsY[i - 1] + CHUNK_MAX_HEIGHT);
//	}
//	m_ChunkCutsY.push_back(m_Height);
//
//	for (int y = 0;y < m_ChunkCountY; y++) {
//		for (int x = 0;x < m_ChunkCountX;x++) {
//			m_Chunks[y * m_ChunkCountX + x]->ResetBoundary(
//				m_ChunkCutsX[x],
//				m_ChunkCutsY[y],
//				m_ChunkCutsX[x + 1],
//				m_ChunkCutsY[y + 1]);
//		}
//
//	}
//		
//}


void World::Render(float deltaTime)
{
	m_VAO->Bind();
	
	m_MainFBO->Bind(GL_FRAMEBUFFER);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// 输出具体状态
	}
	m_BaseShader->Bind();
	m_srcTexture2D->Bind(0);
	glViewport(0, 0, m_Width, m_Height);
	glClear(GL_COLOR_BUFFER_BIT);
	

	for (int y = 0;y < m_ChunkCountY;++y)
	{
		for (int x = 0;x < m_ChunkCountX;++x)
		{
			if (!m_Chunks[y * m_ChunkCountX + x]->is_Dirty)
				continue;
			m_Chunks[y * m_ChunkCountX + x]->UpdateTexData(m_Cells, deltaTime);
			int chunk_width = m_Chunks[y * m_ChunkCountX + x]->end_x - m_Chunks[y * m_ChunkCountX + x]->start_x;
			int chunk_height = m_Chunks[y * m_ChunkCountX + x]->end_y - m_Chunks[y * m_ChunkCountX + x]->start_y;
			int textureXChunk = m_Chunks[y * m_ChunkCountX + x]->start_x;
			int textureYChunk = m_Height - m_Chunks[y * m_ChunkCountX + x]->end_y;;
			m_srcTexture2D->SubImage2D(textureXChunk, textureYChunk, chunk_width, chunk_height, m_Chunks[y * m_ChunkCountX + x]->TexData.data());
			m_Chunks[y * m_ChunkCountX + x]->is_Dirty = false;
		}
	}
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	m_ExtractFBO->Bind(GL_FRAMEBUFFER);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// 输出具体状态
	}
	m_ExtractShader->Bind();
	m_MainTexture2D->Bind(0);
	glViewport(0, 0, m_Width, m_Height);

	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	bool horizontal = true, first_iteration = true;
	int amount = 10;
	m_BlurShader->Bind();
	for (unsigned int i = 0;i < amount;i++)
	{
		m_pingpongFBO[horizontal]->Bind(GL_FRAMEBUFFER);
		m_BlurShader->SetUniform1i("u_horizontal", horizontal);
		first_iteration ? m_ExtractTexture2D->Bind(0) : m_pingpongTexture2D[!horizontal]->Bind(0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, 1024, 1024);

	glClear(GL_COLOR_BUFFER_BIT);
	m_CombineShader->Bind(); 
	m_MainTexture2D->Bind(0);
	m_pingpongTexture2D[!horizontal]->Bind(1);
	m_CombineShader->SetUniform1i("u_scene", 0);
	m_CombineShader->SetUniform1i("u_bloomBlur", 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void World::SetCircleCells(int worldx, int worldy, int radius, Cell cell)
{
	int startX = std::max(0,worldx - radius);
	int endX = std::min(m_Width,worldx + radius);
	int startY = std::max(0,worldy - radius);
	int endY = std::min(m_Height,worldy + radius);
	for (int x = startX; x < endX;++x)
	{
		for (int y = startY; y < endY;++y)
		{
			int dx = x - worldx;
			int dy = y - worldy;
			if (dx * dx + dy * dy <= radius * radius) {
				int rect_x = x >> ACTIVE_RECT_SHIFT;
				int rect_y = y >> ACTIVE_RECT_SHIFT;
				int chunk_x = x >> CHUNK_SHIFT;
				int chunk_y = y >> CHUNK_SHIFT;
				m_ActiveRects[rect_y * (m_Width >> ACTIVE_RECT_SHIFT) + rect_x] = 5;
				m_Chunks[chunk_y * m_ChunkCountX + chunk_x]->is_Dirty = true;
				m_Cells[y * m_Width + x] = cell;
			}
			else
				continue;
		}
	}
}

void World::SetRandomScaleCells(int worldx, int worldy, int radius,Cell cell)
{
	int startX = std::max(0, worldx - radius);
	int endX = std::min(m_Width, worldx + radius);
	int startY = std::max(0, worldy - radius);
	int endY = std::min(m_Height, worldy + radius);
	for (int x = startX; x < endX;++x)
	{
		for (int y = startY; y < endY;++y)
		{
			int dx = x - worldx;
			int dy = y - worldy;
			if (FastRandRange(rand_state, 100) < 4 && dx * dx + dy * dy <= radius * radius)
			{
				int rect_x = x >> ACTIVE_RECT_SHIFT;
				int rect_y = y >> ACTIVE_RECT_SHIFT;
				int chunk_x = x >> CHUNK_SHIFT;
				int chunk_y = y >> CHUNK_SHIFT;
				m_ActiveRects[rect_y * (m_Width >> ACTIVE_RECT_SHIFT) + rect_x] = 5;
				m_Chunks[chunk_y * m_ChunkCountX + chunk_x]->is_Dirty = true;
				m_Cells[y * m_Width + x] = cell;
			}
			else
				continue;
		}
	}
}

