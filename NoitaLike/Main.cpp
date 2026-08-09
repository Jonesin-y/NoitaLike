#include"pch.h"
#include <windows.h>
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include"Game.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;
float FIXED_FRAME = 1 / 144.0f;
Game NoitaLike(512.0f, 512.0f, WINDOW_WIDTH, WINDOW_HEIGHT, "NoitaLike");
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// Ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	}

}
void EnableOpenGLDebugging()
{
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);

		// glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwWindowShouldClose(window);
	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			NoitaLike.Keys[key] = true;
		else if (action == GLFW_RELEASE)
			NoitaLike.Keys[key] = false;
	}
}
void mouse_button_callback(GLFWwindow* window, int button,int action,int mods)
{
	if (button >= 0 && button <= 1024)
	{
		if (action == GLFW_PRESS)
			NoitaLike.MouseButton[button] = true;
		else if (action == GLFW_RELEASE)
			NoitaLike.MouseButton[button] = false;
	}
}
void cursor_pos_callback(GLFWwindow* window,double xpos,double ypos)
{
	NoitaLike.CursorX = xpos;
	NoitaLike.CursorY = ypos;

}
int main()
{
	SetConsoleOutputCP(CP_UTF8);
	GLFWwindow* window;
	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_VISIBLE,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

	window = glfwCreateWindow(1024,1024,"NoitaLike",NULL,NULL);
	if (!window)
	{
		glfwTerminate();
		printf("window不存在!\n");
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Glad加载失败\n");
		return -1;
	}
	if (!NoitaLike.Init())
		printf("游戏初始化失败!\n");
	EnableOpenGLDebugging();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetKeyCallback(window,key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window,cursor_pos_callback);

	glClearColor(0.1f,0.1f,0.1f,1.0f);
	float accumulator = 0.0f;
	float deltaTime = 0.0f;
	float currentTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - currentTime;
		currentTime = glfwGetTime();
		accumulator += deltaTime;
		glfwPollEvents();
		if (accumulator >= FIXED_FRAME)
		{
			accumulator -= FIXED_FRAME;
			NoitaLike.ProcessInput();
			NoitaLike.Update();
		}
		glClear(GL_COLOR_BUFFER_BIT);
		NoitaLike.Render(deltaTime);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}