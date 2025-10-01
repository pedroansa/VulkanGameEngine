#pragma once
#include "AppPipeline.h"
#include "EngineDevice.h"
#include "GameObject.h"
#include "FrameInfo.h"

#include <memory>
#include <vector>
#include <array>

namespace app {
	class InitialRenderSystem
	{
		struct SimplePushConstantData {
			glm::mat4 transform{ 1.f };
			glm::mat4 normalMatrix{ 1.f };
		};


	public: 

		InitialRenderSystem(EngineDevice& device, VkRenderPass renderPass);
		~InitialRenderSystem();

		InitialRenderSystem(const InitialRenderSystem&) = delete;
		InitialRenderSystem& operator=(const InitialRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>&  gameObjects);


	private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass& renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<AppPipeline> appPipeline;
		VkPipelineLayout pipelineLayout;
	};
}


