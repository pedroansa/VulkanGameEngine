#include "InitialRenderSystem.h"

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
		PipelineConfigInfo pipelineConfig{};
		AppPipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		appPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig);
	}
	void InitialRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera &camera)
	{
		// Render
		appPipeline->bind(commandBuffer);
		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : gameObjects) {
			obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
			obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = projectionView * obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);

		}
	}
}