#pragma once
#include"Core.h"
#include<string>
#include<glm/glm.hpp>
#include<glm/matrix.hpp>
class World;
class Game
{
public:
	Game(int width, int height, int windowWidth,int w什indowHeight,const std::string& name);
	bool MouseButton[16] = { false };
	bool Keys[1024] = { false };
	bool ProcessedKeys[1024] = { false };
	double CursorX, CursorY;
	bool Init();
	void ProcessInput();
	void Render();
	void Update();
private:
	uint32_t m_Material;
	Ref(World) m_World;
	std::string m_Name;
	int m_Width, m_Height;
	int m_WindowWidth, m_WindowHeight;
};

