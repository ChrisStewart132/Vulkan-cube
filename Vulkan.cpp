#include "Vulkan.h"





Vulkan::Vulkan(EngineCS *enginePtr, Window* windowPtr)
{//init glfw window
	engine = enginePtr;
	windowClass = windowPtr;
	window = windowClass->getWindow();
	width = windowClass->getWidth();
	height = windowClass->getHeight();
	extent = { width,height };
}

Vulkan::~Vulkan()
{

	vkQueueWaitIdle(queue);
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);

	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkDestroyBuffer(device, vertexBuffer, nullptr);

	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyFramebuffer(device, swapChainFramebuffer, nullptr);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroyImageView(device, swapChainImageViews[0], nullptr);
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);
}

void Vulkan::init()
{
	createInstance();
	getDevice();
	createSwapChain();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createCommandBuffers();
	createSemaphores();
}

void Vulkan::run()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		input();
		drawFrame();
		//windowClass->getfps();
	}
	vkDeviceWaitIdle(device);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Vulkan::createInstance()//instance, surface
{
	const char* extensionNames[2] = { "VK_KHR_surface", "VK_KHR_win32_surface" };//
	VkInstanceCreateInfo createInfo = {};
	createInfo.pApplicationInfo = nullptr;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	createInfo.enabledExtensionCount = 2;
	createInfo.ppEnabledExtensionNames = extensionNames;
	check = vkCreateInstance(&createInfo, nullptr, &instance);//instance
	std::cout << check << std::endl;

	check = glfwCreateWindowSurface(instance, window, nullptr, &surface);//surface
	std::cout << check << std::endl;
}

void Vulkan::getDevice()//physical device, device, queue
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	std::cout << "physical devices: " << physicalDeviceCount << std::endl;

	VkPhysicalDevice* physicalDeviceArray = new VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);

	VkPhysicalDeviceProperties deviceProperties = {};

	for (uint32_t i = 0; i < physicalDeviceCount; i++) {
		vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &deviceProperties);
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			physicalDevice = physicalDeviceArray[i];
			std::cout << deviceProperties.deviceName << std::endl;
			delete[] physicalDeviceArray;
			break;
		}
	}

	VkDeviceQueueCreateInfo queueInfo = {};//queue info
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = 0;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = 0;

	const char* deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queueInfo.queueCount;
	createInfo.pQueueCreateInfos = &queueInfo;
	createInfo.enabledExtensionCount = 1;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	check = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);//deivice
	std::cout << check << std::endl;

	vkGetDeviceQueue(device, 0, 0, &queue);//queue
}

void Vulkan::createSwapChain()//swapchain image imageview
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = 1;
	createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;// VK_FORMAT_B8G8R8A8_UNORM
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//single queue for present and graphics
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;//VK_PRESENT_MODE_MAILBOX_KHR,VK_PRESENT_MODE_FIFO_KHR
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	check = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);//swapchain
	std::cout << check << std::endl;

	swapChainImages = new VkImage(imageCount);
	check = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages);
	std::cout << check << std::endl;//image

	VkImageViewCreateInfo createInfo2 = {};
	createInfo2.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo2.image = swapChainImages[0];
	createInfo2.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo2.format = VK_FORMAT_B8G8R8A8_UNORM;
	createInfo2.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo2.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo2.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo2.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo2.subresourceRange.baseMipLevel = 0;
	createInfo2.subresourceRange.levelCount = 1;
	createInfo2.subresourceRange.baseArrayLayer = 0;
	createInfo2.subresourceRange.layerCount = 1;

	swapChainImageViews = new VkImageView(imageCount);
	check = vkCreateImageView(device, &createInfo2, nullptr, swapChainImageViews);
	std::cout << check << std::endl;//image view
}

void Vulkan::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;// VK_FORMAT_B8G8R8A8_UNORM
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	check = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	std::cout << check << std::endl;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
}

std::vector<char> Vulkan::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void Vulkan::createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();

	std::vector<uint32_t> codeAligned(code.size() / 4 + 1);
	memcpy(codeAligned.data(), code.data(), code.size());

	createInfo.pCode = codeAligned.data();

	check = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	std::cout << check << std::endl;
}

void Vulkan::createGraphicsPipeline()//shaders,pipeline
{
	auto vertShaderCode = readFile("vert.spv");
	auto fragShaderCode = readFile("frag.spv");

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	createShaderModule(vertShaderCode, vertShaderModule);
	createShaderModule(fragShaderCode, fragShaderModule);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

	check = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	std::cout << check << std::endl;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	check = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	std::cout << check << std::endl;

	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

void Vulkan::createFramebuffers()
{
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = swapChainImageViews;
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	check = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffer);
	std::cout << check << std::endl;
}

void Vulkan::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = 0;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

	check = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	std::cout << check << std::endl;
}

uint32_t Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
}

void Vulkan::createVertexBuffer()
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory);
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	//
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
}

void Vulkan::createCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	check = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffers);
	std::cout << check << std::endl;

	//below for each imag (only 1 atm)for(;;){}
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	vkBeginCommandBuffer(commandBuffers, &beginInfo);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { 940,560 };
	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffers, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers, 0, 1, &vertexBuffer, offsets);
	//
	vkCmdDraw(commandBuffers, vertices.size(), 1, 0, 0);//vertex count, instance cnt,first vertex,first instance
	vkCmdEndRenderPass(commandBuffers);
	check = vkEndCommandBuffer(commandBuffers);
	std::cout << check << std::endl;
}

void Vulkan::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	check = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	std::cout << check << std::endl;
	check = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore);
	std::cout << check << std::endl;
}

void Vulkan::drawFrame()
{
	//begin render
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);//3rd option=nanosecond timeout wait for each image

	update();
	//record data
	//bind
	//draw
	//unbind

	//end render
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(queue, &presentInfo);
	vkQueueWaitIdle(queue);
}

void Vulkan::update()
{
	//start acquire swapchain image done in run function
	//update called in middle of run();

	//record command buffer vkBeginCommandBuffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	vkBeginCommandBuffer(commandBuffers, &beginInfo);
	//vkCmdBeginRenderPass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;
	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffers, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	//change vertex data
	moveUnits();

	//bind vertex data
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
	size_t bufferSize = sizeof(vertices[0]) * vertices.size();
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), bufferSize);
	vkUnmapMemory(device, vertexBufferMemory);

	vkCmdBindPipeline(commandBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers, 0, 1, &vertexBuffer, offsets);
	// draw
	vkCmdDraw(commandBuffers, vertices.size(), 1, 0, 0);//vertex count, instance cnt,first vertex,first instance
														//vkCmdEndRenderPass
	vkCmdEndRenderPass(commandBuffers);
	check = vkEndCommandBuffer(commandBuffers);
	//std::cout << check << std::endl;
	//vkEndCommandBuffer
	vkEndCommandBuffer(commandBuffers);
	//end submit info done in run function
}

void Vulkan::moveUnits()
{
	int nVertices = 36;
	int lStart = 0;
	//std::cout << "x: " << engine->worldRotation.x << ", y: " << engine->worldRotation.y << std::endl;
	//rotate all vertices (39 vec 3)
	//translate->scale->rotate

	if (windowClass->isKeyPressed(GLFW_KEY_A)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.x -= 0.01f;
		}
		cameraTranslation.x -= 0.01f;
	}
	else if (windowClass->isKeyPressed(GLFW_KEY_D)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.x += 0.01f;
		}
		cameraTranslation.x += 0.01f;
	}
	else if (windowClass->isKeyPressed(GLFW_KEY_W)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.y -= 0.01f;
		}
		cameraTranslation.y -= 0.01f;
	}
	else if (windowClass->isKeyPressed(GLFW_KEY_S)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.y += 0.01f;
		}
		cameraTranslation.y += 0.01f;
	}
	else if (windowClass->isKeyPressed(GLFW_KEY_Q)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.z -= 0.01f;
		}
		cameraTranslation.z -= 0.01f;
	}
	else if (windowClass->isKeyPressed(GLFW_KEY_E)) {
		for (int c = lStart; c < nVertices; c++) {
			vertices[c].pos.z += 0.01f;
		}
		cameraTranslation.z += 0.01f;
	}
	//std::cout << cameraTranslation.z << std::endl;

	if (windowClass->isMousePressed(GLFW_MOUSE_BUTTON_2)) {
		if (cameraMousePosX == 0) {
			cameraMousePosX = mousePosX;
			cameraMousePosY = mousePosY;
		}
		//std::cout << "x: " << mousePosX << ", y: " << mousePosY << std::endl;//0,0 top left corner
		//std::cout << "x: " << mousePosX << ", y: " << mousePosY << std::endl;

		deltaMousePosX = ((mousePosX - cameraMousePosX) / 200.0f);
		deltaMousePosY = ((mousePosY - cameraMousePosY) / 200.0f);

		cameraTranslation = { 0,0,0 };
		if (mousePosX > cameraMousePosX) {
			for (int i = lStart; i < nVertices; i++)
			{
				engine->rotate(vertices[i].pos, -1.0f * deltaMousePosX, 'y', cameraTranslation);
			}
			engine->worldRotation.x += deltaMousePosX;
		}
		else if (mousePosX < cameraMousePosX) {
			for (int i = lStart; i < nVertices; i++)
			{
				engine->rotate(vertices[i].pos, -1.0f * deltaMousePosX, 'y', cameraTranslation);
			}
			engine->worldRotation.x += deltaMousePosX;
		}

		if (mousePosY > cameraMousePosY) {
			for (int i = lStart; i < nVertices; i++)
			{
				engine->rotate(vertices[i].pos, deltaMousePosY, 'x', cameraTranslation);
			}
			engine->worldRotation.y += deltaMousePosY;
		}
		else if (mousePosY < cameraMousePosY) {
			for (int i = lStart; i < nVertices; i++)
			{
				engine->rotate(vertices[i].pos, deltaMousePosY, 'x', cameraTranslation);
			}
			engine->worldRotation.y += deltaMousePosY;
		}

		if (engine->worldRotation.y >= 360.0f) {
			engine->worldRotation.y -= 360.0f;
		}
		else if (engine->worldRotation.y <= -360.0f) {
			engine->worldRotation.y += 360.0f;
		}
		if (engine->worldRotation.x >= 360.0f) {
			engine->worldRotation.x -= 360.0f;
		}
		else if (engine->worldRotation.x <= -360.0f) {
			engine->worldRotation.x += 360.0f;
		}
	}
}

void Vulkan::input()
{
	windowClass->getMousePos(mousePosX, mousePosY);
	//std::cout << "mouse x: " << mousePosX << " ,mouse y: " << mousePosY << std::endl;
}
