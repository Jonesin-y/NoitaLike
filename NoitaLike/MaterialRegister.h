#pragma once
#define NONE  0
#define AIR   1
#define SAND  2
#define WATER 3
#define STONE 4
#define WOOD  5
#define FIRE  6
#define SMOKE 7
#define STEAM 8
#define ACID 9
#define ACID_STEAM 10
namespace Materials
{
	struct Properties
	{
		float density;					//4	密度
		float hardness;					//4	硬度(概率属性)						:硬度越大，物体越不容易受爆炸影响
		int flammability;				//4	易燃性(概率属性)					:易燃性越大，物体越容易被点燃,如果为0则能覆盖火，负值表示不会被火覆盖
		int vaporize;					//4	蒸发性(概率属性)					:蒸发性越大，物体越容易被蒸发	
		int viscosity;					//4 流体粘滞度(概率属性)				:粘滞度越高，固体越不容易在液体中下落
		int slip_probility;				//4 固体在流体中横向移动概率(概率属性)	:概率越大，固体越难在水中横向移动
		int slide_probility;			//4 固体颗粒滑落概率(概率属性)			:概率越大，固体越难斜向滑落
		int turbulence;					//4 湍流(概率属性)						:湍流越大，固体越有可能优先往斜向移动
		int disappear_probility;		//4 消失概率(概率属性)					:概率越大，物体越容易随时间消失
		int smoke_emission;				//4 烟雾排放概率(概率属性)				:概率越大，物体越容易产生烟雾
		int sour;						//4 酸性(概率属性)						:酸性越大，物体越容易腐蚀其它可被腐蚀物
		int corrosion;					//4 腐蚀性(概率属性)					:腐蚀性越大，物体越容易被酸腐蚀
		int gravity;					//4	重力
		int t_speed;					//4 基础竖直移动速率							
		int s_speed;					//4 基础水平移动速率					
		int color_var;					//4	颜色偏移,为了计算方便，该值设置为2的幂次方
		uint32_t color;					//4	颜色
		unsigned int relevant_material;	//4	反应后材质
		bool is_liquid;					//1	是否是流体
		bool is_fire;					//1	是否是火焰
		bool is_gas;					//1	是否是气体
	};
	constexpr Properties Registry[256] =
	{//	 density		hardness	flammablity		vaporize		viscosity	slip_probility		slide_probility		turbulence	disappear_probility		smoke_emossion	sour	corrosion		gravity		t_speed		s_speed		color_var	color							relevant_material	is_liquid	is_fire		is_gas
		{99999.0f,		99999.0f,	0,				0,				99999,		0,					0,					0,			0,						0,				0,		0,				0,			0,			0,			0,			0,								NONE				,false		,false		,false}, //虚无物质 0
		{0.0f,			0.0f,		0,				0,				0,			0,					0,					0,			0,						0,				0,		0,				0,			1,			1,			0,			0,								AIR					,false		,false		,false}, //空气 1
		{2.5f,			2.0f,		0,				0,				0,			50,					30,					0,			0,						0,				0,		50,				1,			3,			2,			16,			194|178<<8|128<<16|255<<24,		SAND				,false		,false		,false}, //沙子 2
		{1.0f,			0.5f,		0,				100,			30,			0,					0,					0,			0,						0,				0,		0,				1,			5,			5,			0,			15 | 94<<8|156<<16|150<<24,		STEAM				,true		,false		,false}, //水 3
		{10.0f,			20.0f,		0,				0,				0,			0,					30,					0,			0,						0,				0,		0,				0,			0,			0,			32,			140|142<<8|145<<16|255<<24,		STONE				,false		,false		,false}, //石头 4
		{1.5f,			3.0f,		30,				0,				0,			0,					0,					0,			0,						0,				0,		75,				0,			0,			0,			32,			100| 65<<8|30 <<16|255<<24,		WOOD				,false		,false		,false}, //木头 5
		{0.3f,			0.3f,		0,				0,				30,			0,					0,					0,			2,						1,				0,		0,				0,			2,			2,			64,			210|90<<8 |10<<16 |254<<24,		FIRE				,false		,true		,false}, //火焰 6
		{-0.5f,			0.3f,		0,				0,				10,			30,					0,					5,			1,						0,				0,		0,				-1,			3,			2,			32,			50 |50<<8 |50<<16 |200<<24,		SMOKE				,false		,false		,true}, //烟雾 7
		{-1.0f,			0.2f,		0,				0,				8,			40,					0,					20,			1,						0,				0,		0,				-1,			3,			3,			4,			122|181<<8|214<<16|100<<24,		STEAM				,false		,false		,true}, //水蒸气 8	 
		{1.2f,			0.5f,		0,				100,			30,			50,					0,					0,			0,						0,				30,		0,				1,			4,			3,			0,			105|135<<8|28 <<16|254<<24,		ACID_STEAM			,true		,false		,false}, //酸液 9
		{-1.0f,			0.5f,		0,				0,				0,			40,					0,					10,			1,						0,				0,		0,				-1,			3,			3,			4,			186|255<<8|125<<16|100<<24,		ACID_STEAM			,false		,false		,true}, //酸液蒸汽 10

		//待补充
	};
}