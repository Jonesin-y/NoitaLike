#pragma once
#define AIR  0
#define SAND 1
#define WATER 2
#define STONE 3
namespace Materials
{
	struct Properties
	{
		bool is_liquid;
		int gravity;
		int color_var;
		float density;
		float hardness;
		uint32_t color;

	};
	constexpr Properties Registry[256] =
	{	//is_liquid		gravity		color_var	density		hardness	color
		{false,			0,			0,			0.0f,		0.0f,		0},//空气
		{false,			1,			16,			1.5f,		2.0f,		194|178<<8|128<<16|255<<24},//沙子
		{true,			1,			0,			1.0f,		0.5f,		15 | 94<<8|156<<16|150<<24},//水
		{false,			0,			32,			10.0f,		20.0f,		140|142<<8|145<<16|255<<24},//石头
		//待补充
	};
}