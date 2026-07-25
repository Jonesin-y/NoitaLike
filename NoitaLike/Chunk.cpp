#include "pch.h"
#include "Chunk.h"
#include "MaterialRegister.h"
#include "Cell.h"
Chunk::Chunk(int width, int height,int chunkX,int chunkY)
	:m_Width(width),m_Height(height),m_ChunkX(chunkX),m_ChunkY(chunkY),is_Dirty(false)
{
	Cells.resize(m_Width * m_Height,Cell(0));
}

std::vector<std::tuple<int, int, Dir>> Chunk::Update()
{
	for (int y = 0;y < m_Height;++y)
	{
		for (int x = 0;x < m_Width;++x)
		{
			if (Cells[y * m_Width + x].GetMaterial() == AIR)
				continue;
			Cells[y * m_Width + x].SetUpdated(false);
		}
	}
	std::vector<std::tuple<int, int, Dir>> requestCells;
	for (int y = 0;y < m_Height;++y)
	{
		for (int x = 0;x < m_Width;++x)
		{

			int material = Cells[y * m_Width + x].GetMaterial();
			int moving = Cells[y * m_Width + x].GetMoving();//获取当前Cell的材质
			int is_Updated = Cells[y * m_Width + x].GetUpdated();
			int current_cell_gravity = Materials::Registry[material].gravity;//计算当前Cell的重力
			float current_cell_density = Materials::Registry[material].density;//计算当前Cell的密度
			bool is_liquid = Materials::Registry[material].is_liquid;//判断当前cell是不是liquid
			if (!moving || is_Updated)
				continue;
			if (current_cell_gravity == 0)
				continue;
			if (y - current_cell_gravity < 0)
			{
				int global_x = m_ChunkX * m_Width + x;
				int global_y = m_ChunkY * m_Height + y;
				requestCells.push_back(std::make_tuple(global_x, global_y, DOWN));
				continue;
			}
			else if (y - current_cell_gravity >= m_Height)
			{
				int global_x = m_ChunkX * m_Width + x;
				int global_y = m_ChunkY * m_Height + y;

				requestCells.push_back(std::make_tuple(global_x, global_y, UP));
				continue;
			}
			auto TrySwap = [&](int t_x, int t_y)->bool
				{
					if (t_x < 0)
					{
						int global_x = m_ChunkX * m_Width + x;
						int global_y = m_ChunkY * m_Height + y;

						requestCells.push_back(std::make_tuple(global_x, global_y, LEFT));
						return false;
					}
					if (t_x >= m_Width)
					{
						int global_x = m_ChunkX * m_Width + x;
						int global_y = m_ChunkY * m_Height + y;

						requestCells.push_back(std::make_tuple(global_x, global_y, RIGHT));
						return false;
					}
					int target_mat = Cells[t_y * m_Width + t_x].GetMaterial();
					float target_den = Materials::Registry[target_mat].density;

					bool canSwap = (current_cell_gravity > 0 && current_cell_density > target_den)
						|| (current_cell_gravity < 0 && current_cell_density < target_den);
					if (canSwap)
					{
						is_Dirty = true;
						Cell temp = Cells[y * m_Width + x];
						Cells[y * m_Width + x] = Cells[t_y * m_Width + t_x];
						Cells[t_y * m_Width + t_x] = temp;
						Cells[t_y * m_Width + t_x].SetUpdated(true);
						Cells[y * m_Width + x].SetUpdated(true);
						return true;
					}
					else
						return false;

				};
			int dir1 = (rand() % 2 == 0) ? -1 : 1;
			int dir2 = -dir1;
			if (TrySwap(x, y - current_cell_gravity)) continue;
			else if (TrySwap(x + dir1, y - current_cell_gravity)) continue;
			else if (TrySwap(x + dir2, y - current_cell_gravity)) continue;
			else if (is_liquid && TrySwap(x + dir1, y)) continue;
			else if (is_liquid && TrySwap(x + dir2, y)) continue;
			//else
				//Cells[y * m_Width + x].SetMoving(false);

			
		}
	}
	return requestCells;
}

void Chunk::UpdateTexData()
{
	TexData.clear();
	auto ClampColor = [](uint32_t value)->uint32_t
		{
			if (value < 0)return 0;
			else if (value > 255)return 255;
			else return value;
		};
	for (int y = m_Height - 1;y >=0;--y)
	{
		for (int x = 0;x < m_Width;++x)
		{
			if (Materials::Registry[Cells[y * m_Width + x].GetMaterial()].color_var == 0) { TexData.push_back(Materials::Registry[Cells[y * m_Width + x].GetMaterial()].color); continue; }
			//因为AIR的color_var为0，不能和其余体素共用一套逻辑，直接传入颜色为纯色就行
			uint32_t initial_color = Materials::Registry[Cells[y * m_Width + x].GetMaterial()].color;
			if(Cells[y * m_Width + x].GetColorVariation() == 0)
				Cells[y * m_Width + x].SetColorVariation(rand() & (Materials::Registry[Cells[y * m_Width + x].GetMaterial()].color_var - 1));
			uint32_t color_raw = Cells[y * m_Width + x].GetColorVariation();
			uint32_t final_R = ClampColor((initial_color & 0xFF) + color_raw);
			uint32_t final_G = ClampColor(((initial_color >> 8) & 0xFF) + color_raw);
			uint32_t final_B = ClampColor(((initial_color >> 16) & 0xFF) + color_raw);
			uint32_t final_A = initial_color >> 24;
			TexData.push_back(final_R | final_G << 8 | final_B << 16 | final_A << 24);
		}
	}
}



bool Chunk::ChunkExist(int px, int py)
{
	return false;
}


