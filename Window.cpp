#include "Window.h"




bool Window::keys[MAX_KEYS];
bool Window::mouse[MAX_MOUSE];
double Window::mx;
double Window::my;

Window::Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Vulkan", nullptr, nullptr);

	glfwSetKeyCallback(glfwWindow, key_callback);
	glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
	glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
	glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, 1);
}


Window::~Window()
{
}

void Window::getfps()
{
	//fps vsync 60fps
	double ctime = glfwGetTime();
	frame++;
	if (ctime - time >= 1.0)
	{
		time += 1.0f;
		std::cout << "fps: " << frame << std::endl;
		frame = 0;
	}
	ctime = 0;
}


void Window::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		keys[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}

int Window::isKeyPressed(int keyCode)
{
	return keys[keyCode];
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		mouse[button] = true;
	}
	if (action == GLFW_RELEASE)
	{
		mouse[button] = false;
	}
}

int Window::isMousePressed(int mouseCode)
{
	return mouse[mouseCode];
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mx = xpos;
	my = ypos;
}

void Window::getMousePos(double& x, double& y)
{
	x = mx;
	y = my;
}