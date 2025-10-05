#include "PointLightSystem.h"
#include <iostream>


namespace app {

	PointLightSystem::PointLightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device }
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		/*VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);*/

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void PointLightSystem::createPipeline(VkRenderPass& renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before layour");

		// Para pipeline sólido (original)
		PipelineConfigInfo solidConfig{};
		PipelineConfigInfo wireframeConfig{};
		PipelineConfigInfo pointConfig{};

		AppPipeline::defaultPipelineConfigInfo(solidConfig);
		auto solidVertexInput = AppPipeline::getSolidVertexInputConfig();
		solidVertexInput.attributeDescriptions.clear();
		solidVertexInput.bindingDescriptions.clear();
		solidConfig.renderPass = renderPass;
		solidConfig.pipelineLayout = pipelineLayout;
		solidPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/POINT_LIGHT/shader.vert.spv",
			"shaders/POINT_LIGHT/shader.frag.spv",
			solidConfig, solidVertexInput);

		////Para wireframe
		//AppPipeline::wireframePipelineConfigInfo(wireframeConfig);
		//auto wireFrameVertexInput = AppPipeline::getWireframeVertexInputConfig();
		//wireframeConfig.renderPass = renderPass;
		//wireframeConfig.pipelineLayout = pipelineLayout;
		//wireframePipeline = std::make_unique<AppPipeline>(
		//	engineDevice,
		//	"shaders/WIREFRAME/vert.spv",
		//	"shaders/WIREFRAME/frag.spv",
		//	wireframeConfig, wireFrameVertexInput);

		////// Para pontos
		//AppPipeline::pointsPipelineConfigInfo(pointConfig);
		//auto pointsVertexInput = AppPipeline::getPointsVertexInputConfig();
		//pointConfig.renderPass = renderPass;
		//pointConfig.pipelineLayout = pipelineLayout;
		//pointsPipeline = std::make_unique<AppPipeline>(
		//	engineDevice,
		//	"shaders/POINTS/vert.spv",
		//	"shaders/POINTS/frag.spv",
		//	pointConfig, pointsVertexInput);
	}
	void PointLightSystem::render(FrameInfo& frameInfo)
	{

		solidPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		// Render
		//auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

		
	}

}