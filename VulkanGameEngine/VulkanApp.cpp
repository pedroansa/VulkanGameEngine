#include "VulkanApp.h"

namespace app {

	struct SimplePushConstantData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	VulkanApp::VulkanApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffer();
	}
	VulkanApp::~VulkanApp()
	{
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	void VulkanApp::run()
	{
		while (!appWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(engineDevice.device());
	}
	void VulkanApp::loadModels()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f,0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
		//sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });

		model = std::make_unique<Model>(engineDevice, vertices);
	}
	void VulkanApp::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void VulkanApp::createPipeline()
	{
		assert(swapChain != nullptr && "Cannot create pipeline before swapChain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before layour");
		PipelineConfigInfo pipelineConfig{};
		AppPipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		appPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig);
	}

	void VulkanApp::createCommandBuffer()
	{
		commandBuffers.resize(swapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = engineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VulkanApp::freeCommandBuffer()
	{
		vkFreeCommandBuffers(
			engineDevice.device(),
			engineDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
	}

	void VulkanApp::recordCommandBuffer(int imageIndex) {
		
		static int frame = 30;
		frame = (frame + 1) % 100;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);


		appPipeline->bind(commandBuffers[imageIndex]);
		model->bind(commandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++) {
			SimplePushConstantData push{};
			push.offset = { -0.5f + 0.02f * frame , -0.4f + j * 0.25f };
			push.color = { 1.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(
				commandBuffers[imageIndex],
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			model->draw(commandBuffers[imageIndex]);

		}
		//model->draw(commandBuffers[imageIndex]);
		// vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		
	}
	void VulkanApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = swapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			appWindow.wasWindowResized()) {
			appWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void VulkanApp::recreateSwapChain()
	{
		auto extent = appWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = appWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(engineDevice.device());

		if (swapChain == nullptr) {
			swapChain = std::make_unique<EngineSwapChain>(engineDevice, extent);
		}
		else {
			swapChain = std::make_unique<EngineSwapChain>(engineDevice, extent, std::move(swapChain));
			if (swapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffer();
				createCommandBuffer();
			}
		}
		createPipeline();
	}

	void VulkanApp::sierpinski(
		std::vector<Model::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
}