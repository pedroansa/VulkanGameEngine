#pragma once
#include "AppPipeline.h"
#include "EngineDevice.h"
#include "GameObject.h"
#include "Camera.h"
#include <memory>
#include <vector>
#include <array>

namespace app {
	class InitialRenderSystem
	{
		struct SimplePushConstantData {
			glm::mat4 transform{ 1.f };
			glm::mat4 modelMatrix{ 1.f };
		};


	public: 

		InitialRenderSystem(EngineDevice& device, VkRenderPass renderPass);
		~InitialRenderSystem();

		InitialRenderSystem(const InitialRenderSystem&) = delete;
		InitialRenderSystem& operator=(const InitialRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>&  gameObjects, const Camera &camera);


	private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass& renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<AppPipeline> appPipeline;
		VkPipelineLayout pipelineLayout;
	};
}


