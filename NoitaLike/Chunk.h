#pragma once
#include"Cell.h"
#include<cstdint>
#include<vector>
#include<functional>
#include"Core.h"
enum Dir
{
	LEFT,RIGHT,UP,DOWN
};
class Chunk
{
public:
	bool is_Dirty;
	std::vector<uint32_t>TexData;
	std::vector<Cell>Cells;
	Chunk(int width,int height,int chunkX,int chunkY);
	std::vector<std::tuple<int,int,Dir>> Update();
	void UpdateTexData();
	bool ChunkExist(int px,int py);
private:
	int m_Width, m_Height;
	int m_ChunkX, m_ChunkY;
	

};

