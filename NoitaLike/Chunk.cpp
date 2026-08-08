#include "pch.h"
#include "Chunk.h"
#include "MaterialRegister.h"
#include "Cell.h"

const int ACTIVE_RECT_LENGTH = 32;
const int ACTIVE_RECT_SHIFT = 5;

Chunk::Chunk(int startx, int starty, int endx, int endy, int worldwidth, int worldheight)
	:world_width(worldwidth), world_height(worldheight), m_Width(endx - startx), m_Height(endy - starty), m_Toogle(false), is_Dirty(true)
{

	start_x = std::max(startx, 0);//max属于动态边界逻辑，暂时不用
	start_y = std::max(starty,0);//max属于动态边界逻辑，暂时不用
	end_x = std::min(endx, world_width);//min属于动态边界逻辑，暂时不用
	end_y = std::min(endy, world_height);//min属于动态边界逻辑，暂时不用
	start_rect_x = start_x >> ACTIVE_RECT_SHIFT;
	start_rect_y = start_y >> ACTIVE_RECT_SHIFT;
	end_rect_x = end_x >> ACTIVE_RECT_SHIFT;
	end_rect_y = end_y >> ACTIVE_RECT_SHIFT;
	
}
void Chunk::Update(std::vector<Cell>& global_cells,std::vector<uint8_t>& active_rects,unsigned int current_frame)
{
	m_Toogle = !m_Toogle;

	int active_rect_count_x = world_width >> ACTIVE_RECT_SHIFT;
	int active_rect_count_y = world_height >> ACTIVE_RECT_SHIFT;
	auto GetRelevantRectIndex = [&](int x, int y)->int
		{
			int rect_x = x >> ACTIVE_RECT_SHIFT;
			int rect_y = y >> ACTIVE_RECT_SHIFT;
			return rect_y * active_rect_count_x + rect_x;
		}; 

	int leftOrRight = m_Toogle;
	int startPoint = leftOrRight ? start_x : end_x - 1;
	int endPoint = leftOrRight ? end_x : start_x - 1;
	int step = leftOrRight ? 1 : -1;
	for (int y = start_y >> ACTIVE_RECT_SHIFT;y < end_y >> ACTIVE_RECT_SHIFT;++y)
	{
		for (int x = start_x >> ACTIVE_RECT_SHIFT;x < end_x >> ACTIVE_RECT_SHIFT;++x)
		{
			active_rects[y * active_rect_count_x + x] = std::max(0, active_rects[y * active_rect_count_x + x] - 1);
		}
	}
	//不涉及交换逻辑，按活跃矩形为单位更新Updated标签
	for (int y = start_y;y < end_y;++y)
	{
		for (int x = startPoint;x != endPoint;x += step)
		{

			if (active_rects[GetRelevantRectIndex(x, y)] == 0) {
				if (step == 1)
					x = std::min(x + ACTIVE_RECT_LENGTH - 1, end_x - 1);
				else if (step == -1)
					x = std::max(x - ACTIVE_RECT_LENGTH + 1, start_x);
				continue;
			}
			global_cells[y * world_width + x].SetUpdated(false);
		}
	}
	//涉及交换逻辑，活跃矩形作为掩码检查是否需要更新当前体素，避免边界问题
	for (int y = start_y;y < end_y;++y)
	{
		
		for (int x = startPoint;x != endPoint;x+=step)
		{
			if (active_rects[GetRelevantRectIndex(x, y)] == 0) {
				if (step == 1)
					x = std::min(x + ACTIVE_RECT_LENGTH-1, end_x - 1);
				else if (step == -1)
					x = std::max(x - ACTIVE_RECT_LENGTH+1, start_x);
				continue;
			}

			int material = global_cells[y * world_width + x].GetMaterial();
			int moving = global_cells[y * world_width + x].GetMoving();//获取当前Cell的材质
			int cell_current_frame = global_cells[y * world_width + x].GetCurrentFrame();

			if (!moving || cell_current_frame == current_frame)//如果当前Cell没有移动或者已经被处理过了，就不处理
				continue;

			int current_cell_turbulence = Materials::Registry[material].turbulence;
			int current_cell_gravity = Materials::Registry[material].gravity;//计算当前Cell的重力
			int current_cell_slide_probility = Materials::Registry[material].slide_probility;//计算当前Cell的滑落概率
			int current_cell_slip_probility = Materials::Registry[material].slip_probility;//计算当前Cell在水中的平移概率
			int current_cell_fall_speed = Materials::Registry[material].fall_speed;
			int current_cell_flow_speed = Materials::Registry[material].flow_speed;
			float current_cell_density = Materials::Registry[material].density;//计算当前Cell的密度
			bool is_liquid = Materials::Registry[material].is_liquid;//判断当前cell是不是流体

			
			if (current_cell_gravity == 0)
				continue;
			
			auto TrySwap = [&](int global_curr_x,int global_curr_y,int global_tar_x, int global_tar_y)->bool
				{
					
					//获取当前Cell的信息
					if (global_tar_x < 0 || global_tar_x >= world_width || global_tar_y < 0 || global_tar_y >= world_height)//越界了就不处理
						return false;
					int   curr_mat = global_cells[global_curr_y * world_width + global_curr_x].GetMaterial();
					int   curr_gen = Materials::Registry[curr_mat].gravity;
					bool  curr_liquid = Materials::Registry[curr_mat].is_liquid;
					int   curr_viscosity = Materials::Registry[curr_mat].viscosity;
					float curr_den = Materials::Registry[curr_mat].density;
					//获取目标Cell的信息
					int   tar_mat = global_cells[global_tar_y * world_width + global_tar_x].GetMaterial();
					bool  tar_liquid = Materials::Registry[tar_mat].is_liquid;
					int   tar_viscosity = Materials::Registry[tar_mat].viscosity;
					float tar_den = Materials::Registry[tar_mat].density;

					bool canSwap = (curr_gen > 0 && curr_den > tar_den)
						|| (curr_gen < 0 && curr_den < tar_den);
					
					if (canSwap)
					{
						if (tar_liquid && (rand() % 100) < tar_viscosity)//如果被液体阻滞就不下降
							return true;
						auto WakeUp = [&](int global_x,int global_y)->bool
							{
								if (global_x < 0 || global_x >= world_width|| global_y < 0 || global_y >= world_height)//越界了就不处理
									return false;
								Cell* wCell = &global_cells[global_y * world_width + global_x];

								wCell->SetFreeFalling(true);
								wCell->SetSliping(true);
								active_rects[GetRelevantRectIndex(global_x, global_y)] = 5;
								//wCell->SetMoving(true);
								return true;
								};
						is_Dirty = true;
						if (tar_mat == AIR || (tar_liquid && curr_den > tar_den))
						{
							global_cells[global_curr_y * world_width + global_curr_x].SetCurrentFrame(current_frame);
						}
						else
						{
							global_cells[global_tar_y * world_width + global_tar_x].SetCurrentFrame(current_frame);
							global_cells[global_curr_y * world_width + global_curr_x].SetCurrentFrame(current_frame);
						}
						global_cells[global_curr_y * world_width + global_curr_x].SetUpdated(true);
						global_cells[global_tar_y * world_width + global_tar_x].SetUpdated(true);
						
						Cell temp = global_cells[global_curr_y * world_width + global_curr_x];
						global_cells[global_curr_y * world_width + global_curr_x] = global_cells[global_tar_y * world_width + global_tar_x];
						global_cells[global_tar_y * world_width + global_tar_x] = temp;

						WakeUp(global_tar_x + 1, global_tar_y);
						WakeUp(global_tar_x - 1, global_tar_y);
						WakeUp(global_tar_x, global_tar_y + 1);
						WakeUp(global_tar_x, global_tar_y - 1);
						//WakeUp(global_tar_x + 1, global_tar_y + 1);
						//WakeUp(global_tar_x - 1, global_tar_y - 1);
						//WakeUp(global_tar_x - 1, global_tar_y + 1);
						//WakeUp(global_tar_x + 1, global_tar_y - 1);

						WakeUp(global_curr_x + 1, global_curr_y);
						WakeUp(global_curr_x - 1, global_curr_y);
						WakeUp(global_curr_x, global_curr_y + 1);
						WakeUp(global_curr_x, global_curr_y - 1);
						//WakeUp(global_curr_x + 1, global_curr_y + 1);
						//WakeUp(global_curr_x - 1, global_curr_y - 1);
						//WakeUp(global_curr_x - 1, global_curr_y + 1);
						//WakeUp(global_curr_x + 1, global_curr_y - 1);

						return true;
					}
					else
						return false;

				};
				auto RayCastMove = [&](int global_start_x, int global_start_y, int global_target_x, int global_target_y)->bool//返回true表示被处理
					{
						if (global_start_x == global_target_x && global_start_y == global_target_y) return false;

						int dx = std::abs(global_target_x - global_start_x);
						int dy = std::abs(global_target_y - global_start_y);
						int sx = (global_target_x - global_start_x) > 0 ? 1 : -1;
						int sy = (global_target_y - global_start_y) > 0 ? 1 : -1;
						int err = dx - dy;

						int global_current_x = global_start_x;//记录当前检测位置的游标
						int global_current_y = global_start_y;//记录当前检测位置的游标

						int global_last_valid_x = global_start_x;
						int global_last_valid_y = global_start_y;
						//沿着射线步进探测
						while (true) {
							// 前进一步 (Bresenham 核心逻辑)
							int e2 = 2 * err;
							if (e2 > -dy) { err -= dy; global_current_x+=sx;}
							if (e2 < dx) { err += dx;  global_current_y+=sy;}
							// 检查越界（直接当做撞墙处理）
							if (global_current_x < 0 || global_current_x >= world_width || global_current_y < 0 || global_current_y >= world_height)
								break;
							// 碰撞检测逻辑（复用之前的密度判定）
							int check_mat = global_cells[global_current_y * world_width + global_current_x].GetMaterial();	//不越界时的默认值
							bool check_locked = (global_cells[global_current_y * world_width + global_current_x].GetCurrentFrame() == current_frame);//不越界时的默认值
							int check_is_liquid = Materials::Registry[check_mat].is_liquid;
							float check_den= Materials::Registry[check_mat].density;
							if (check_locked)
							{
								bool can_ignore_locked = check_is_liquid && !is_liquid && current_cell_density > check_den;
								// 如果目标不能忽视锁死，直接撞墙停止
								if (!can_ignore_locked)
									break;
							}
							// 检查是否能穿透（空气，或者密度更小的液体）
							bool can_penetrate = (check_mat == AIR) ||
								(current_cell_density >= check_den&& Materials::Registry[check_mat].is_liquid);

							if (can_penetrate) {
								// 安全，记录为最后一个合法落脚点
								global_last_valid_x = global_current_x;
								global_last_valid_y = global_current_y;
							}
							else {
								// 撞到不可穿透的硬物了，停止射线
								break;
							}

							// 如果到达了理想终点，结束探测
							if (global_current_x == global_target_x && global_current_y == global_target_y) break;
						}

						//执行单次传送交换
						if (global_last_valid_x != global_start_x || global_last_valid_y != global_start_y) {
							// 调用之前的 TrySwap，但现在只执行一次远距离 Swap
							return TrySwap(global_start_x,global_start_y,global_last_valid_x, global_last_valid_y);
						}

						return false;
					};
			int dir1 = (rand() % 2 == 0) ? -1 : 1;
			int dir2 = -dir1;
			if (rand() % 100 < current_cell_turbulence)//如果当前Cell有流动，在优先往斜向移动
			{
				if (RayCastMove(x,y,x + (dir1 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
					continue;
				else if (RayCastMove(x,y,x + (dir2 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
					continue;
				else if (RayCastMove(x, y, x, y - current_cell_gravity * current_cell_fall_speed))
				{
					if (!is_liquid)
					{
						if (rand() % 100 < current_cell_slide_probility)//如果小于当前cell的滑动概率，就将其FreeFalling设置为false,反之则斜向滑动
							global_cells[y * world_width + x].SetFreeFalling(false);

					}
					continue;
				}
				else if (is_liquid && RayCastMove(x, y, x + (dir1 * current_cell_flow_speed),y))
					continue;
				else if (is_liquid && RayCastMove(x, y, x + (dir2 * current_cell_flow_speed), y))
					continue;
				//else
					//active_rect_data[GetRelevantRectIndex(x, y)] -= 1;

			}
			else if((y - 1) >= 0 && (y - 1) < world_height && global_cells[(y-1) * world_width + x].GetUpdated())//如果下方的体素刚刚被处理过，就优先尝试斜向交换
			{
				if (RayCastMove(x, y, x + (dir1 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
					continue;
				else if (RayCastMove(x, y, x + (dir2 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
					continue;
				else if (RayCastMove(x, y, x, y - current_cell_gravity * current_cell_fall_speed))
				{
					if (!is_liquid)
					{
						if (rand() % 100 < current_cell_slide_probility)//如果小于当前cell的滑动概率，就将其FreeFalling设置为false,反之则斜向滑动
							global_cells[y * world_width + x].SetFreeFalling(false);

					}
					continue;
				}
				else if (is_liquid && RayCastMove(x, y, x + (dir1 * current_cell_flow_speed), y))
					continue;
				else if (is_liquid && RayCastMove(x, y, x + (dir2 * current_cell_flow_speed), y))
					continue;
				//else
					//active_rect_data[GetRelevantRectIndex(x, y)] -= 1;

			}
			else
			{
				if (RayCastMove(x, y, x, y - current_cell_gravity * current_cell_fall_speed))
					continue;
				if (!is_liquid)
				{
					if (rand() % 100 < current_cell_slide_probility)//如果小于当前cell的滑动概率，就将其FreeFalling设置为false,反之则斜向滑动
						global_cells[y * world_width + x].SetFreeFalling(false);

				}
				if (global_cells[y * world_width + x].GetFreeFalling())
				{
					if (RayCastMove(x, y, x + (dir1 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
						continue;
					else if (RayCastMove(x, y, x + (dir2 * current_cell_flow_speed), y - current_cell_gravity * current_cell_fall_speed))
						continue;
				}
				if (is_liquid && RayCastMove(x, y, x + (dir1 * current_cell_flow_speed), y)) 	continue;
				else if (is_liquid && RayCastMove(x, y, x + (dir2 * current_cell_flow_speed), y))continue;
				//else
					//active_rect_data[GetRelevantRectIndex(x, y)] -= 1;

			}
			if (!is_liquid)
			{
				auto TrySlip = [&](int slip_x) {
					int side_mat;
					int neighbor_slip_x;
					if(slip_x < 0 || slip_x >= world_width)
						return false;
					if (slip_x >= 0 && slip_x < world_width) {
						side_mat = global_cells[y * world_width + slip_x].GetMaterial();
						// 只有当侧面是液体，且自身密度大于侧面液体密度时，才允许水平挤压
					}
					
					if (Materials::Registry[side_mat].is_liquid && current_cell_density > Materials::Registry[side_mat].density) {
						return RayCastMove(x, y, slip_x, y);

					}
					return false;
					};
				if (rand() % 100 < current_cell_slip_probility){//如果小于当前cell的水平移动概率就将其Sliping设置为false,反之则横向滑动
					global_cells[y * world_width + x].SetSliping(false);
					continue;
				}
				// 尝试向左右两侧的水中溃散
				if (global_cells[y * world_width + x].GetSliping())
				{
					if (TrySlip(x + dir1 * current_cell_flow_speed))
						continue;
					else if (TrySlip(x + dir2 * current_cell_flow_speed))
						continue; 
				}
			}
			//active_rect_data[GetRelevantRectIndex(x, y)] -= 1;
			
			
		}
	}
};

void Chunk::UpdateTexData(std::vector<Cell>& global_cells)
{
	TexData.clear();
	auto ClampColor = [](uint32_t value)->uint32_t
		{
			if (value < 0)return 0;
			else if (value > 255)return 255;
			else return value;
		};
	for (int y = end_y-1;y >=start_y;--y)
	{
		for (int x = start_x;x < end_x;++x)
		{
			if (Materials::Registry[global_cells[y * world_width + x].GetMaterial()].color_var == 0) { TexData.push_back(Materials::Registry[global_cells[y * world_width + x].GetMaterial()].color); continue; }
			//因为AIR的color_var为0，不能和其余体素共用一套逻辑，直接传入颜色为纯色就行
			uint32_t initial_color = Materials::Registry[global_cells[y * world_width + x].GetMaterial()].color;
			if( global_cells[y * world_width + x].GetColorVariation() == 0)
				global_cells[y * world_width + x].SetColorVariation(rand() & (Materials::Registry[global_cells[y * world_width + x].GetMaterial()].color_var - 1));
			uint32_t color_raw = global_cells[y * world_width + x].GetColorVariation();
			uint32_t final_R = ClampColor((initial_color & 0xFF) + color_raw);
			uint32_t final_G = ClampColor(((initial_color >> 8) & 0xFF) + color_raw);
			uint32_t final_B = ClampColor(((initial_color >> 16) & 0xFF) + color_raw);
			uint32_t final_A = initial_color >> 24;
			TexData.push_back(final_R | final_G << 8 | final_B << 16 | final_A << 24);
		}
	}
}

void Chunk::ResetBoundary(int startx, int starty, int endx, int endy)
{
	start_x = std::max(startx, 0);
	start_y = std::max(starty, 0);
	end_x = std::min(endx, world_width);
	end_y = std::min(endy, world_height);
	is_Dirty = true;
}


