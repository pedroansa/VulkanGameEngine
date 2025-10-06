#include "AppRenderer.h"

namespace app {
	AppRenderer::AppRenderer(AppWindow& window, EngineDevice& device) : appWindow{window}, engineDevice{device}, currentImageIndex(0),        // INICIALIZAR
		currentFrameIndex(0),        // INICIALIZAR  
		isFrameStarted(false)        // INICIALIZAR
	{
		recreateSwapChain();
		createCommandBuffer();
	}
	AppRenderer::~AppRenderer()
	{
		freeCommandBuffer();
	}

	void AppRenderer::createCommandBuffer()
	{
		commandBuffers.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT );

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

	void AppRenderer::freeCommandBuffer()
	{
		vkFreeCommandBuffers(
			engineDevice.device(),
			engineDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
	}

	void AppRenderer::recreateSwapChain()
	{
		// We need to improve this part (alerta gambiarra)
		int width = 0, height = 0;
		glfwGetFramebufferSize(appWindow.getGLFWwindow(), &width, &height);
		auto extent = appWindow.getExtent();
		while (width == 0 || height == 0 || appWindow.isMinimized()) {
			glfwGetFramebufferSize(appWindow.getGLFWwindow(), &width, &height);
			extent = appWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(engineDevice.device());

		if (swapChain == nullptr) {
			swapChain = std::make_unique<EngineSwapChain>(engineDevice, extent);
		}
		else {
			std::shared_ptr<EngineSwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<EngineSwapChain>(engineDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
				throw std::runtime_error("SwapChain image or depth format has change");
			}
		}
		// TO DO CREATE PIPELINE
	}

	VkCommandBuffer AppRenderer::beginFrame() {
		assert(!isFrameStarted && "Cannot call beginFrame if it's already in progress");

		auto result = swapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;

	}
	void AppRenderer::endFrame() {
		assert(isFrameStarted && "Cannot call endFrame if frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			appWindow.wasWindowResized()) {
			appWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;

		currentFrameIndex = (currentFrameIndex + 1) % EngineSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void AppRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin renderPass on commandBuffer from different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	}
	void AppRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame is in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end renderPass on commandBuffer from different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}