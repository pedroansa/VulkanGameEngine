#pragma once
#include "AppWindow.h"
#include "AppPipeline.h"
#include "EngineDevice.h"
#include "EngineSwapChain.h"
#include <memory>
#include <vector>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace app {
	class VulkanApp
	{
		struct SimplePushConstants {
			glm::vec2 offset;
			glm::vec2 color;
	};

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		VulkanApp();
		~VulkanApp();

		VulkanApp(const VulkanApp&) = delete;
		VulkanApp& operator=(const VulkanApp&) = delete;

		void run();

	private:
		void loadModels();
		void sierpinski(std::vector<Model::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffer();
		void freeCommandBuffer();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		AppWindow appWindow{ WIDTH, HEIGHT, "Vulkan Game Engine" };
		EngineDevice engineDevice{ appWindow };
		std::unique_ptr<EngineSwapChain> swapChain;
		std::unique_ptr<AppPipeline> appPipeline;

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<Model> model;
	};
}


