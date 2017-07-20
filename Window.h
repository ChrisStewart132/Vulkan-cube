#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#define MAX_KEYS 1024
#define MAX_MOUSE 24



class Window
{
private:
	uint32_t windowWidth = 1000, windowHeight = 1000;
	double time = glfwGetTime();
	double frame = 0;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static bool keys[MAX_KEYS];

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static bool mouse[MAX_MOUSE];

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static double mx, my;//mouse pos

	GLFWwindow* glfwWindow;

public:
	GLFWwindow* getWindow() { return glfwWindow; }
	uint32_t getWidth() { return windowWidth; }
	uint32_t getHeight() { return windowHeight; }
	Window();
	~Window();
	void getfps();
	int isKeyPressed(int keyCode);
	int isMousePressed(int mouseCode);
	static void getMousePos(double& x, double& y);
};


