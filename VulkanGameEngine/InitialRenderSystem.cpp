#include "InitialRenderSystem.h"

namespace app {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

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
	void InitialRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects)
	{
		// Update to make it rotate
		//int i = 0;
		//for (auto& obj : gameObjects) {
		//	i += 1;
		//	obj.transform2d.rotation = 
		//		glm::mod(obj.transform2d.rotation + 0.001f, glm::two_pi<float>());
		//}

		// Render
		appPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			//obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();

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
	void InitialRenderSystem::sierpinski(
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