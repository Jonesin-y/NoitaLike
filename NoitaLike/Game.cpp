#include "pch.h"
#include "Game.h"
#include "MaterialRegister.h"
#include "Texture2D.h"
#include "World.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include <GLFW/glfw3.h>
bool show_imgui_demo = true;
const ImVec4 COLOR_NONE = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
const ImVec4 COLOR_AIR = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
const ImVec4 COLOR_SAND = ImVec4(0.7608f, 0.6980f, 0.5020f, 1.0000f);
const ImVec4 COLOR_WATER = ImVec4(0.0588f, 0.3686f, 0.6118f, 0.5882f);
const ImVec4 COLOR_STONE = ImVec4(0.5490f, 0.5569f, 0.5686f, 1.0000f);
const ImVec4 COLOR_WOOD = ImVec4(0.3922f, 0.2549f, 0.1176f, 1.0000f);
const ImVec4 COLOR_FIRE = ImVec4(0.8235f, 0.3529f, 0.0392f, 0.9961f);
const ImVec4 COLOR_SMOKE = ImVec4(0.1961f, 0.1961f, 0.1961f, 0.7843f);
const ImVec4 COLOR_STEAM = ImVec4(0.4784f, 0.7098f, 0.8392f, 0.3922f);
const ImVec4 COLOR_ACID = ImVec4(0.2549f, 0.7647f, 0.1098f, 0.9961f);
const ImVec4 COLOR_ACID_STEAM = ImVec4(0.7294f, 1.0000f, 0.4902f, 0.3922f);

namespace
{
	bool ColoredSquareButton(const char* id, const ImVec4& color, const ImVec2& size)
	{
		ImVec4 normal = color;
		normal.w = 0.55f;

		ImVec4 hovered = color;
		hovered.w = 0.78f;

		ImVec4 active = color;
		active.w = 1.0f;

		ImGui::PushStyleColor(ImGuiCol_Button, normal);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

		const bool clicked = ImGui::Button(id, size);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		return clicked;
	}
}

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
	
}

void Game::Render(float deltaTime)
{
	m_World->Render(deltaTime);
}

void Game::ImGuiRender()
{
	m_World->OnImguiRender();
	DrawRightToolbar();
	ImGui::ShowDemoWindow(&show_imgui_demo);

}

void Game::DrawRightToolbar()
{
	const ImVec2 button_size(30.0f,30.0f);
	static const char* selected_material_name = nullptr;
	static const float text_dur = 1.2f;
	static double button_notice_end_time = ImGui::GetTime();
	static bool show_inventory = false;
	static bool show_settings = false;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	const ImVec2 toolbar_pos(
		viewport->WorkPos.x + viewport->WorkSize.x - 16.0f,
		viewport->WorkPos.y + 16.0f
	);

	ImGui::SetNextWindowPos(
		toolbar_pos,
		ImGuiCond_Always,
		ImVec2(1.0f, 0.0f)
	);

	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	ImGui::Begin("##RightToolBar", nullptr, flags);

	if (ColoredSquareButton("##SAND", COLOR_SAND, button_size))
	{
		selected_material_name = "SAND";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = SAND;
	}

	if (ColoredSquareButton("##WATER", COLOR_WATER, button_size))
	{
		selected_material_name = "WATER";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = WATER;
	}

	if (ColoredSquareButton("##STONE", COLOR_STONE, button_size))
	{
		selected_material_name = "STONE";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = STONE;
	}

	if (ColoredSquareButton("##WOOD", COLOR_WOOD, button_size))
	{
		selected_material_name = "WOOD";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = WOOD;
	}

	if (ColoredSquareButton("##FIRE", COLOR_FIRE, button_size))
	{
		selected_material_name = "FIRE";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = FIRE;
	}

	if (ColoredSquareButton("##SMOKE", COLOR_SMOKE, button_size))
	{
		selected_material_name = "SMOKE";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = SMOKE;
	}

	if (ColoredSquareButton("##STEAM", COLOR_STEAM, button_size))
	{
		selected_material_name = "STEAM";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = STEAM;
	}

	if (ColoredSquareButton("##ACID", COLOR_ACID, button_size))
	{
		selected_material_name = "ACID";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = ACID;
	}

	if (ColoredSquareButton("##ACID_STEAM", COLOR_ACID_STEAM, button_size))
	{
		selected_material_name = "ACID STEAM";
		button_notice_end_time = ImGui::GetTime() + text_dur;
		m_Material = ACID_STEAM;
	}

	ImGui::End();

	ImGui::PopStyleVar(3);
	if (selected_material_name != nullptr && ImGui::GetTime() < button_notice_end_time)
	{
		ImGuiViewport* main_viewport = ImGui::GetMainViewport();

		const ImVec2 notice_pos(
			main_viewport->WorkPos.x + main_viewport->WorkSize.x * 0.5,
			main_viewport->WorkPos.y + 20.0f
		);

		ImGui::SetNextWindowPos(
			notice_pos,
			ImGuiCond_Always,
			ImVec2(0.5f, 0.0f)
		);

		const ImGuiWindowFlags notice_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoDocking;
		
		ImGui::PushStyleVar(
			ImGuiStyleVar_WindowPadding,
			ImVec2(20.0f, 10.0f)
		);
		ImGui::PushStyleVar(
			ImGuiStyleVar_WindowRounding,
			4.0f
		);
		ImGui::PushStyleColor(
			ImGuiCol_Text,
			ImVec4(0.86f, 0.86f, 0.86f, 1.0f)
		);
		ImGui::PushFont(nullptr, 28.0f);

		ImGui::Begin(
			"##MaterialSelectedName",
			nullptr,
			notice_flags
		);

		ImGui::TextUnformatted(selected_material_name);

		ImGui::End();
		
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}
	//if (show_inventory)
	//{ 
	//	ImGui::SetNextWindowSize(
	//		ImVec2(500.0f, 400.0f),
	//		ImGuiCond_FirstUseEver
	//	);
	//	if (ImGui::Begin("Inventory", &show_inventory)) {
	//		//点击之后的后续的处理逻辑
	//
	//	}
	//	ImGui::End();
	//}
	//
	//if (show_settings)
	//{
	//	ImGui::SetNextWindowSize(
	//		ImVec2(500.0f, 400.0f),
	//		ImGuiCond_FirstUseEver
	//	);
	//	if (ImGui::Begin("Settings", &show_settings)){
	//		//点击之后的后续处理逻辑
	//
	//	}
	//	ImGui::End();
	//}
}

		

void Game::Update()
{
	m_World->Update();
}
