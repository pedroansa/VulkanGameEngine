#pragma once
#include "AppPipeline.h"
#include "EngineDevice.h"
#include "GameObject.h"
#include <memory>
#include <vector>
#include <array>

namespace app {
	class InitialRenderSystem
	{
		struct SimplePushConstantData {
			glm::mat4 transform{ 1.f };
			alignas(16) glm::vec3 color;
		};


	public: 

		InitialRenderSystem(EngineDevice& device, VkRenderPass renderPass);
		~InitialRenderSystem();

		InitialRenderSystem(const InitialRenderSystem&) = delete;
		InitialRenderSystem& operator=(const InitialRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>&  gameObjects);


	private:
		void sierpinski(std::vector<Model::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

		void createPipelineLayout();
		void createPipeline(VkRenderPass& renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<AppPipeline> appPipeline;
		VkPipelineLayout pipelineLayout;
	};
}


