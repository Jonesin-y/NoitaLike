#pragma once
#include"Core.h"
#include"Cell.h"
#include"Chunk.h"
#include <vector>
class Texture2D;
class VertexArray;
class Shader;
class World
{
public:
	World(int width,int height);
	void Update();
	void SolveAllRequest();
	void Render();
	void SetCircleCells(int worldx, int worldy, int radius, Cell cell);

private:
	Ref(Texture2D)m_Texture2D;
	Ref(VertexArray)m_VAO;
	Ref(Shader)m_Shader;
	std::vector<Ref(Chunk)>m_Chunks;
	std::vector<std::tuple<int, int, Dir>> m_AllRequests;
	int m_Width, m_Height;
	int m_ChunkCountX, m_ChunkCountY;
};

