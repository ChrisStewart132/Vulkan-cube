#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>						//vectors mats
#include <glm/gtc/matrix_transform.hpp>		//transformation matrix
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <array>

#include "EngineCS.h"
#include "Window.h"


struct Vertex {
	glm::vec3 pos;
	glm::vec3 colour;
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, colour);
		return attributeDescriptions;
	}
};


static std::vector<Vertex> vertices = {
	// cube
	{ { 0.2f,	0.2f,	-0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.2f,	-0.2f,	-0.2f },{ 1.0f,  0.0f, 0.0f } },
	{ { -0.2f,	-0.2f,	-0.2f },{ 1.0f,  0.0f, 0.0f } },
	{ { -0.2f,  -0.2f,	-0.2f },{ 1.0f,  0.0f, 0.0f } },
	{ { -0.2f,	0.2f,	-0.2f },{ 1.0f,  0.0f, 0.0f } },
	{ { 0.2f,   0.2f,	-0.2f },{ 1.0f,  0.0f, 0.0f } },
	//
	{ { -0.2f,	-0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.2f,	-0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.2f,	0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.2f,  0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { -0.2f,	0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	{ { -0.2f, -0.2f,	0.2f },{ 1.0f, 0.0f, 0.0f } },
	//
	{ { -0.2f,	-0.2f,	0.2f },{ 0.0f,  0.0f,  1.0f } },
	{ { -0.2f,	0.2f,	0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { -0.2f,	0.2f,	-0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { -0.2f,  0.2f,	-0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { -0.2f,	-0.2f,	-0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { -0.2f, -0.2f,	0.2f },{ 0.0f,  0.0f, 1.0f } },
	//
	{ { 0.2f,	-0.2f,	-0.2f },{ 0.0f,  0.0f,  1.0f } },
	{ { 0.2f,	0.2f,	-0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { 0.2f,	0.2f,	0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { 0.2f,  0.2f,	0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { 0.2f,	-0.2f,	0.2f },{ 0.0f,  0.0f, 1.0f } },
	{ { 0.2f,  -0.2f,	-0.2f },{ 0.0f,  0.0f, 1.0f } },
	//bottom
	{ { 0.2f,	0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { 0.2f,	0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { -0.2f,	0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { -0.2f,  0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { -0.2f,	0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { 0.2f,   0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	//top
	{ { -0.2f,	-0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { 0.2f,	-0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { 0.2f,	-0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { 0.2f,   -0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { -0.2f,	-0.2f,	0.2f },{ 0.0f,  1.0f, 0.0f } },
	{ { -0.2f,   -0.2f,	-0.2f },{ 0.0f,  1.0f, 0.0f } },
};


static struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
}ubo;




class Vulkan
{

private://members
	VkResult check;//vk result

	EngineCS* engine;

	GLFWwindow* window;//window 
	Window* windowClass;//window class ptr

	glm::vec3 cameraTranslation = { 0.0f ,0.0f,0.0f };
	double mousePosX, mousePosY, cameraMousePosX = 0, cameraMousePosY = 0;
	float xVelocity, yVelocity;
	float deltaMousePosX = 0, deltaMousePosY = 0;
	
	uint32_t width, height;
	VkExtent2D extent;

	VkInstance instance;								//vkDestroyInstance(instance,nullptr);
	VkSurfaceKHR surface;								//vkDestroySurfaceKHR(instance,nullptr);

	VkPhysicalDevice physicalDevice;
	VkDevice device;									//vkDestroyDevice(device,nullptr);
	VkQueue queue;

	VkSwapchainKHR swapChain;							//vkDestroySwapchainKHR(device,swapChain,nullptr);
	uint32_t imageCount = 1;							//swapchainimages only using 1 image, 
	VkImage *swapChainImages;							//delete[] swapChainImages;				delete[] stuff done in vulkan api?
	VkImageView *swapChainImageViews;					//delete[] swapChainImageViews;			vkDestroyImageView(device,&swapChainImageViews,nullptr);

	VkRenderPass renderPass;							//vkDestroyRenderPass(device,renderPass,nullptr);

	VkPipelineLayout pipelineLayout;					//vkDestroyPipelineLayout(device,pipelineLayout,nullptr);
	VkPipeline graphicsPipeline;						//vkDestroyPipeline(device,graphicsPipeline,nullptr);

	VkFramebuffer swapChainFramebuffer;					//vkDestroyFramebuffer(device,swapChainFramebuffer,nullptr);

	VkCommandPool commandPool;							//vkDestroyCommandPool(device,commandPool,nullptr);		
	VkBuffer vertexBuffer;								//vkDestroyBuffer(device,vertexBuffer,nullptr);
	VkDeviceMemory vertexBufferMemory;					//vkFreeMemory(device,vertexBufferMemory.nullptr);

	VkCommandBuffer commandBuffers;						//deleted with commandPool auto

	VkSemaphore imageAvailableSemaphore;				//vkDestroySemaphore(device,imageAvailableSemaphore,nullptr);
	VkSemaphore renderFinishedSemaphore;				//vkDestroySemaphore(device,renderFinishedSemaphore,nullptr);

public:
	Vulkan(EngineCS* enginePtr, Window* _window);
	~Vulkan();
	void init();
	void run();

private://functions
	void createInstance();
	void getDevice();
	void createSwapChain();
	void createRenderPass();
	static std::vector<char> Vulkan::readFile(const std::string& filename);//for shader/pipeline
	void createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);//
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);//for vertexBuffer 
	void createVertexBuffer();
	void createCommandBuffers();
	void createSemaphores();


	void drawFrame();
	void update();

	void moveUnits();

	void input();

};

