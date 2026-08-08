#include "pch.h"
#include "Game.h"
#include "MaterialRegister.h"
#include "World.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
Game::Game(int width, int height, int windowWidth, int windowHeight,const std::string& name)
	:m_Width(width),m_Height(height),m_WindowWidth(windowWidth),m_WindowHeight(windowHeight),m_Name(name),m_Material(SAND)
{
	
}

bool Game::Init()
{
	m_World = std::make_shared<World>(m_Width, m_Height);
	return true;
}

void Game::ProcessInput()
{
	if (MouseButton[GLFW_MOUSE_BUTTON_1] == true)
	{
		glm::mat4 projection(1.0f);
		glm::mat4 view(1.0f);
		projection = glm::ortho(0.0f, (float)m_Width, (float)m_Height, 0.0f, -1.0f, 1.0f);
		glm::mat4 inverseProjection = glm::inverse(projection);
		glm::mat4 inverseView = glm::inverse(view);
		double ndc_x = CursorX / m_WindowWidth * 2 - 1;
		double ndc_y = CursorY / m_WindowHeight* 2 - 1;
		glm::vec4 ndcPos = glm::vec4(ndc_x, ndc_y, 0.0f, 1.0f);
		glm::vec4 FinalCursorPos = inverseProjection * inverseView * ndcPos;
		//printf("鼠标左键触发!坐标:%d,%d\n", (int)FinalCursorPos.x, (int)FinalCursorPos.y);
		m_World->SetRandomScaleCells((int)FinalCursorPos.x, (int)FinalCursorPos.y, 20,m_Material);
		//m_World->SetCircleCells((int)FinalCursorPos.x, (int)FinalCursorPos.y, 5, m_Material);
	}
	if (Keys[GLFW_KEY_0] == true && ProcessedKeys[GLFW_KEY_0] == false)
	{
		m_Material = 1;
		std::cout << m_Material << std::endl;
		ProcessedKeys[GLFW_KEY_0] = true;
	}
	else if (Keys[GLFW_KEY_0] == false && ProcessedKeys[GLFW_KEY_0] == true)
		ProcessedKeys[GLFW_KEY_0] = false;

	if (Keys[GLFW_KEY_1] == true && ProcessedKeys[GLFW_KEY_1] == false)
	{
		m_Material = 2;
		std::cout << m_Material << std::endl;
		ProcessedKeys[GLFW_KEY_1] = true;
	}
	else if (Keys[GLFW_KEY_1] == false && ProcessedKeys[GLFW_KEY_1] == true)
		ProcessedKeys[GLFW_KEY_1] = false;

	if (Keys[GLFW_KEY_2] == true && ProcessedKeys[GLFW_KEY_2] == false)
	{
		m_Material = 3;
		std::cout << m_Material << std::endl;
		ProcessedKeys[GLFW_KEY_2] = true;
	}
	else if (Keys[GLFW_KEY_2] == false && ProcessedKeys[GLFW_KEY_2] == true)
		ProcessedKeys[GLFW_KEY_2] = false;

	if (Keys[GLFW_KEY_3] == true && ProcessedKeys[GLFW_KEY_3] == false)
	{
		m_Material = 4;
		std::cout << m_Material << std::endl;
		ProcessedKeys[GLFW_KEY_3] = true;
	}
	else if (Keys[GLFW_KEY_3] == false && ProcessedKeys[GLFW_KEY_3] == true)
		ProcessedKeys[GLFW_KEY_3] = false;

	if (Keys[GLFW_KEY_4] == true && ProcessedKeys[GLFW_KEY_4] == false)
	{
		m_Material = 5;
		std::cout << m_Material << std::endl;
		ProcessedKeys[GLFW_KEY_4] = true;
	}
	else if (Keys[GLFW_KEY_4] == false && ProcessedKeys[GLFW_KEY_4] == true)
		ProcessedKeys[GLFW_KEY_4] = false;

}

void Game::Render()
{
	m_World->Render();
}

void Game::Update()
{
	m_World->Update();
}
