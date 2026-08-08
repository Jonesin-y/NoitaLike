#pragma once
#define NONE  0
#define AIR   1
#define SAND  2
#define WATER 3
#define STONE 4
#define GRIT  5
namespace Materials
{
	struct Properties
	{
		float density;		//4	密度
		float hardness;		//4	硬度(概率属性)						:硬度越大，物体越不容易受爆炸影响
		int viscosity;		//4 流体粘滞度(概率属性)				:粘滞度越高，固体越不容易在液体中下落
		int slip_probility; //4 固体在流体中横向移动概率(概率属性)	:概率越大，固体越难在水中横向移动
		int slide_probility;//4 固体颗粒滑落概率(概率属性)			:概率越大，固体越难斜向滑落
		int turbulence;		//4 湍流(概率属性)						:湍流越大，固体越有可能优先往斜向移动
		int gravity;		//4	重力
		int fall_speed;		//4 基础下落速率							
		int flow_speed;		//4 基础水平移动速率					:
		int color_var;		//4	颜色偏移
		uint32_t color;		//4	颜色
		bool is_liquid;		//1	是否是流体
	};
	constexpr Properties Registry[256] =
	{//	 density		hardness	viscosity	slip_probility		slide_probility		turbulence	gravity		fallspeed		flow_speed		color_var	color						is_liquid
		{99999.0f,		99999.0f,	99999,		0,					0,					0,			0,			0,				0,				0,			0							,false		},//虚无物质
		{0.0f,			0.0f,		0,			0,					0,					0,			0,			1,				1,				0,			0							,false		},//空气
		{1.5f,			2.0f,		0,			50,					30,					0,			1,			3,				2,				16,			194|178<<8|128<<16|255<<24	,false		},//沙子
		{1.0f,			0.5f,		30,			0,					0,					0,			1,			5,				5,				0,			15 | 94<<8|156<<16|150<<24	,true		},//水
		{10.0f,			20.0f,		0,			0,					30,					0,			0,			0,				0,				32,			140|142<<8|145<<16|255<<24	,false		},//石头
		{1.5f,			1.5f,		0,			50,					10,					0,			1,			3,				2,				32,			110|112<<8|155<<16|255<<24	,false		} //砂砾
		//待补充
	};
}