#pragma once

#include"Core.h"
#include"Cell.h"
#include"Chunk.h"
#include <vector>
class Texture2D;
class VertexArray;
class Shader;
class Cell;
class World
{
public:
	World(int width,int height);
	void Update();
	//void UpdateChunkBoundary(int offset);动态边界核心函数
	void Render();
	void SetCircleCells(int worldx, int worldy, int radius, Cell cell);
	void SetRandomScaleCells(int worldx,int worldy, int radius,Cell cell);

private:
	Ref(Texture2D)m_Texture2D;
	Ref(VertexArray)m_VAO;
	Ref(Shader)m_Shader;
	
	std::vector<Ref(Chunk)>m_Chunks;
	std::vector<Ref(Chunk)>m_RedChunks;
	std::vector<Ref(Chunk)>m_BlueChunks;
	std::vector<Ref(Chunk)>m_YellowChunks;
	std::vector<Ref(Chunk)>m_GreenChunks;
	std::vector<Cell>m_Cells;
	//std::vector<int>m_ChunkCutsX;动态边界逻辑核心变量
	//std::vector<int>m_ChunkCutsY;动态边界逻辑核心变量
	std::vector<uint8_t>m_ActiveRects;
	//std::vector<uint16_t> m_ActiveRectData;
	int m_Width, m_Height;
	int m_ChunkCountX, m_ChunkCountY;
	int m_ActiveRectCountX, m_ActiveRectCountY;
};

