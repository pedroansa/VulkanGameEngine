#pragma once
#include "AppWindow.h"
#include "EngineDevice.h"
#include "EngineSwapChain.h"
#include <memory>
#include <vector>
#include <array>
#include <cassert>

namespace app {
	class AppRenderer
	{
	public:
		AppRenderer(AppWindow& window, EngineDevice& device);
		~AppRenderer();

		AppRenderer(const AppRenderer&) = delete;
		AppRenderer& operator=(const AppRenderer&) = delete;

		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get commandBuffer if frame is not in progress");
			return commandBuffers[currentFrameIndex]; }

		int getCurrentFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index if frame is not in progress");
			return currentFrameIndex;
		}

		float getAspectRatio() const { return swapChain->extentAspectRatio(); }
		VkRenderPass getSwapChainRenderPass() const{ return swapChain->getRenderPass(); }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


	private:
		void createCommandBuffer();
		void freeCommandBuffer();
		void recreateSwapChain();

		AppWindow& appWindow;
		EngineDevice& engineDevice;
		std::unique_ptr<EngineSwapChain> swapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}


