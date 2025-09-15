#include "VulkanApp.h"

namespace app {

	struct SimplePushConstantData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	VulkanApp::VulkanApp()
	{
		loadGameObjects();
		createPipelineLayout();
		createPipeline();
	}
	VulkanApp::~VulkanApp()
	{
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	void VulkanApp::run()
	{
		while (!appWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = appRenderer.beginFrame()) {
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
				appRenderer.endSwapChainRenderPass(commandBuffer);
				appRenderer.endFrame();

			}
		}

		vkDeviceWaitIdle(engineDevice.device());
	}
	void VulkanApp::loadGameObjects()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f,0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
		//sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		auto model = std::make_shared<Model>(engineDevice, vertices);
		
		std::vector<glm::vec3> colors{
		  {1.f, .7f, .73f}
		};
		for (auto& color : colors) {
			color = glm::pow(color, glm::vec3{ 2.2f });
		}

		for (int i = 0; i < 1; i++) {
			auto triangle = GameObject::createGameObject();
			triangle.model = model;
			//triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
			triangle.color = colors[i % colors.size()];
			gameObjects.push_back(std::move(triangle));
		}
	}
	void VulkanApp::createPipelineLayout()
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

	void VulkanApp::createPipeline()
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before layour");
		PipelineConfigInfo pipelineConfig{};
		AppPipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = appRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		appPipeline = std::make_unique<AppPipeline>(
			engineDevice,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig);
	}
	void VulkanApp::renderGameObjects(VkCommandBuffer commandBuffer)
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
	void VulkanApp::sierpinski(
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