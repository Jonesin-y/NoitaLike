#pragma once
#include<cstdint>
//24-31位表示材质，最多可表示256种材质
//16-23位表示颜色偏移，避免同一种材质为一种单调的颜色,也许后续会调整
//8-15位表示动态槽位
//0-7位表示体素基础属性


class Cell
{
public:
	uint32_t cell;
	Cell(uint32_t c) :cell(c << 24)
	{
		cell |= 1U << 5;
		cell &= (~(1U << 6));
		cell |= 1U << 7;
	}
	//Setter
	inline void SetMaterial(unsigned int type){cell = (cell & (~(0xFFU << 24))) | ((type&0xFFU) << 24);}
	inline void SetColorVariation(unsigned int colorvariation){cell = (cell & (~(0xFFU << 16))) | (colorvariation << 16);}//暂时删除，后续也许会修改这八位的作用
	inline void SetMoving(bool is_moving) 
	{
		if(is_moving)
			cell |= 1U << 7;
		if(!is_moving)
			cell &= (~(1U << 7));
	}
	inline void SetUpdated(bool is_updated)//Updated的语义转变，不再是一帧只能移动一次的标签，而是判断同一帧内刚刚是否已经被处理过的标签
	{
		if (is_updated)
			cell |= 1U << 6;
		if (!is_updated)
			cell &= (~(1U << 6));
	}//因为y轴向上，设置是否更新标签是为了防止气体体素在一帧内被连续处理
	inline void SetFreeFalling(bool freeFalling)
	{
		if (freeFalling)
			cell |= 1U << 5;
		if (!freeFalling)
			cell &= (~(1U << 5));
	}
	inline void SetSliping(bool is_sliping)
	{
		if (is_sliping)
			cell |= 1U << 4;
		if (!is_sliping)
			cell &= (~(1U << 4));
	}
	inline void SetCurrentFrame(unsigned int currentFrame) { cell = (cell & (~(0xFFU << 8))) | ((currentFrame & 0xFFU) << 8); }

	inline void ChangeMoving() { cell ^= (1U << 7); }
	//Getter
	inline bool GetMoving() { return (cell >> 7) & 1U; }
	inline bool GetUpdated() { return (cell >> 6) & 1U; }
	inline bool GetFreeFalling() { return (cell >> 5) & 1U; }
	inline bool GetSliping() { return (cell >> 4) & 1U; }


	inline unsigned int GetMaterial(){return (cell >> 24) & 0xFFU;}
	inline unsigned int GetColorVariation(){return (cell >> 16) & 0xFFU;}
	inline unsigned int GetCurrentFrame() { return (cell >> 8) & 0xFFU; }
};


