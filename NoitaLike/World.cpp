#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Buffer.h"
#include "World.h"
#include "MaterialRegister.h"
#include "Chunk.h"
#include "Cell.h"
const int CHUNK_WIDTH = 512;
const int CHUNK_HEIGHT = 512;
World::World(int width,int height)
	:m_Width(width),m_Height(height)
{
	m_Shader = std::make_shared<Shader>("Assets/Shader/test.vs", "Assets/Shader/test.fs","TestShader");
	m_VAO = std::make_shared<VertexArray>();
	m_Texture2D = std::make_shared<Texture2D>(m_Width, m_Height);
	m_Texture2D->Bind(0);
	for (int x = 0;x < 1;++x)
	{
		for (int y = 0;y<1;++y)
		{
			m_Chunks.push_back(std::make_shared<Chunk>(CHUNK_WIDTH, CHUNK_WIDTH, x, y));
		}
	}
	m_ChunkCountX = m_Width  / CHUNK_WIDTH;
	m_ChunkCountY = m_Height / CHUNK_HEIGHT;

	glm::mat4 model(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(m_Width, m_Height, 0.0f));
	projection = glm::ortho(0.0f, (float)m_Width, (float)m_Height, 0.0f, -1.0f, 1.0f);
	m_Shader->SetUniformMat4f("u_model", 1, GL_FALSE, &model[0][0]);
	m_Shader->SetUniformMat4f("u_view", 1, GL_FALSE, &view[0][0]);
	m_Shader->SetUniformMat4f("u_projection", 1, GL_FALSE, &projection[0][0]);
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
	for (int x = 0;x<m_ChunkCountX;++x)
	{
		for (int y = 0;y < m_ChunkCountY;++y)
		{
			std::vector<std::tuple<int, int, Dir>> RequestCells = m_Chunks[y * m_ChunkCountX + x]->Update();
			m_Chunks[y * m_ChunkCountX + x]->UpdateTexData();
			m_AllRequests.insert(m_AllRequests.end(), RequestCells.begin(), RequestCells.end());
		}
	}
}

void World::SolveAllRequest()
{
	for (auto& request : m_AllRequests)
	{
		int global_x = std::get<0>(request);
		int global_y = std::get<1>(request);
		Dir direction = std::get<2>(request);
		int chunkX = global_x >> 9;
		int chunkY = global_y >> 9;
		int local_x = global_x & 511;
		int local_y = global_y & 511;
		int mat = m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x].GetMaterial();
		int gra = Materials::Registry[mat].gravity;
		int den = Materials::Registry[mat].density;

		auto TrySwap = [&](int t_chunkX, int t_chunkY, int t_x, int t_y)->bool
			{
				int t_mat = m_Chunks[t_chunkY * m_ChunkCountX + t_chunkX]->Cells[t_y * CHUNK_WIDTH + t_x].GetMaterial();
				int t_den = Materials::Registry[t_mat].density;

				bool can_swap = gra > 0 && den > t_den || gra < 0 && den < t_den;
				if (can_swap)
				{
					m_Chunks[chunkY * m_ChunkCountX + chunkX]->is_Dirty = true;
					m_Chunks[t_chunkY * m_ChunkCountX + t_chunkX]->is_Dirty = true;
					Cell temp = m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x];
					m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x] = m_Chunks[t_chunkY * m_ChunkCountX + t_chunkX]->Cells[t_y * CHUNK_WIDTH + t_x];
					m_Chunks[t_chunkY * m_ChunkCountX + t_chunkX]->Cells[t_y * CHUNK_WIDTH + t_x] = temp;
					m_Chunks[t_chunkY * m_ChunkCountX + t_chunkX]->Cells[t_y * CHUNK_WIDTH + t_x].SetUpdated(true);
					m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x].SetUpdated(true);
					return true;
				}
				else
					return false;
			};
		
		int global_tar_x = global_x;
		int global_tar_y = global_y;
		switch (direction)
		{
		case LEFT:  global_tar_x = global_x - 1;break;
		case RIGHT:	global_tar_x = global_x + 1;break;
		case DOWN:  global_tar_y = global_y - 1;break;
		case UP:	global_tar_y = global_y + 1;break;
		}
		int local_tar_x = global_tar_x & 511;
		int local_tar_y = global_tar_y & 511;
		int tar_chunkX = global_tar_x >> 9;
		int tar_chunkY = global_tar_y >> 9;
		if (global_tar_x < 0 || global_tar_x >= m_Width || global_tar_y < 0 || global_tar_y >= m_Height)
		{
			m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x].SetMoving(false);
			continue;
		}
		if(TrySwap(tar_chunkX,tar_chunkY,global_tar_x,global_tar_y))
			m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x].SetMoving(true);
		else
			m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[local_y * CHUNK_WIDTH + local_x].SetMoving(false);
	}
	m_AllRequests.clear();
}

void World::Render()
{
	m_Shader->Bind();
	m_VAO->Bind();
	m_Texture2D->Bind(0);
	
	for (int x = 0;x < m_ChunkCountX;++x)
	{
		for (int y = 0;y < m_ChunkCountY;++y)
		{
			if (!m_Chunks[y * m_ChunkCountX + x]->is_Dirty)
				continue;
			m_Texture2D->SubImage2D(x * CHUNK_WIDTH,y*CHUNK_HEIGHT,CHUNK_WIDTH,CHUNK_HEIGHT,m_Chunks[y * m_ChunkCountX + x]->TexData.data());
			m_Chunks[y * m_ChunkCountX + x]->is_Dirty = false;
		}
	}
	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
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
			if (dx * dx + dy * dy <= radius * radius)
			{
				int localX = x & 511;
				int localY = y & 511;
				int chunkX = x >> 9;
				int chunkY = y >> 9;
				m_Chunks[chunkY * m_ChunkCountX + chunkX]->Cells[localY * CHUNK_WIDTH + localX] = cell;
				m_Chunks[chunkY * m_ChunkCountX + chunkX]->is_Dirty = true;

			}
			else
				continue;
		}
	}
}
