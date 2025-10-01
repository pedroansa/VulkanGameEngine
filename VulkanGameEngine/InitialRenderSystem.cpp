#include "InitialRenderSystem.h"
#include <iostream>


namespace app {

	InitialRenderSystem::InitialRenderSystem(EngineDevice& device, VkRenderPass renderPass) : engineDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}
	InitialRenderSystem::~InitialRenderSystem()
	{
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void InitialRenderSystem::createPipelineLayout()
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

	void InitialRenderSystem::createPipeline(VkRenderPass& renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before layour");

		// Para pipeline sólido (original)
		PipelineConfigInfo solidConfig{};
		PipelineConfigInfo wireframeConfig{};
		PipelineConfigInfo pointConfig{};

		AppPipeline::defaultPipelineConfigInfo(solidConfig);
		auto solidVertexInput = AppPipeline::getSolidVertexInputConfig();
		solidConfig.renderPass = renderPass;
		solidConfig.pipelineLayout = pipelineLayout;
		solidPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/SOLID/vert.spv",
			"shaders/SOLID/frag.spv",
			solidConfig, solidVertexInput);

		//Para wireframe
		AppPipeline::wireframePipelineConfigInfo(wireframeConfig);
		auto wireFrameVertexInput = AppPipeline::getWireframeVertexInputConfig();
		wireframeConfig.renderPass = renderPass;
		wireframeConfig.pipelineLayout = pipelineLayout;
		wireframePipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/WIREFRAME/vert.spv",
			"shaders/WIREFRAME/frag.spv",
			wireframeConfig, wireFrameVertexInput);

		//// Para pontos
		AppPipeline::pointsPipelineConfigInfo(pointConfig);
		auto pointsVertexInput = AppPipeline::getPointsVertexInputConfig();
		pointConfig.renderPass = renderPass;
		pointConfig.pipelineLayout = pipelineLayout;
		pointsPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/POINTS/vert.spv",
			"shaders/POINTS/frag.spv",
			pointConfig, pointsVertexInput);
	}
	void InitialRenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{

		switch (pipelineMode) {
		case PipelineMode::SOLID:
			solidPipeline->bind(frameInfo.commandBuffer);
			break;
		case PipelineMode::WIREFRAME:
			wireframePipeline->bind(frameInfo.commandBuffer);
			break;
		case PipelineMode::POINTS:
			pointsPipeline->bind(frameInfo.commandBuffer);
			break;
		}

		// Render
		auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

		for (auto& obj : gameObjects) {
			//obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
			//std::cout << obj.transform.rotation.z << std::endl;
			//obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());*/

			SimplePushConstantData push{};
			auto modelMatrix = obj.transform.mat4();
			push.transform = projectionView * modelMatrix;
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);

		}
	}

}