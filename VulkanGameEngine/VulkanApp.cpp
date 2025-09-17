#include "VulkanApp.h"

namespace app {

	VulkanApp::VulkanApp()
	{
		loadGameObjects();
	}

	VulkanApp::~VulkanApp(){}

	void VulkanApp::run()
	{
		InitialRenderSystem initialRenderSystem{ engineDevice, appRenderer.getSwapChainRenderPass() };
		while (!appWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = appRenderer.beginFrame()) {
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				initialRenderSystem.renderGameObjects(commandBuffer, gameObjects);
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