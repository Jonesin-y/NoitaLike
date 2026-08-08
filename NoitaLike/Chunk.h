#pragma once
#include"Cell.h"
#include<cstdint>
#include<vector>
#include<functional>
#include"Core.h"
enum class MoveResult
{
	MOVED,
	STATIC_BLOCKED,
	RETRY_LATER,
	QUEUED,
};
class Chunk
{
public:
	int world_rect_count_x, world_rect_count_y;
	int world_width, world_height;
	int start_x,start_y,start_rect_x,start_rect_y;
	int end_x, end_y,end_rect_x,end_rect_y;
	bool is_Dirty;
	std::vector<uint32_t>TexData;
	Chunk(int startx,int starty,int endx,int endy,int worldwidth,int worldheight);
	void Update(std::vector<Cell>& global_cells,std::vector<uint8_t>& active_rects,unsigned int current_frame);
	void UpdateTexData(std::vector<Cell>& global_cells);
	void ResetBoundary(int startx,int starty,int endx,int endy);
	
	

private:
	int m_Width, m_Height;
	int m_ChunkX, m_ChunkY;
	int m_Toogle;//用于计算是偶数帧还是奇数帧
	

};

