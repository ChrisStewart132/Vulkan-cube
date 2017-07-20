//vulkan version 1.0.51
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "EngineCS.h"
#include "Window.h"
#include "Vulkan.h"


uint32_t main() {
	EngineCS engine;
	Window glfwWindow;
	Vulkan vulkan(&engine, &glfwWindow);
	vulkan.init();
	vulkan.run();
	return 0;
}