#include "VulkanApp.h"
#include <iostream>
#include "AppBuffer.h"

namespace app {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	VulkanApp::VulkanApp()
	{
		loadGameObjects();
	}

	VulkanApp::~VulkanApp(){}

	void VulkanApp::run()
	{
		std::vector<std::unique_ptr<AppBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<AppBuffer>(engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		Camera camera{};
		auto viewerObject = GameObject::createGameObject();
		viewerObject.transform.translation = { -1.f, -1.0f, 2.0f };
		glm::vec3 directionToOrigin = glm::normalize(glm::vec3(0.0f) - viewerObject.transform.translation);
		viewerObject.transform.rotation.y = atan2(directionToOrigin.x, directionToOrigin.z); // yaw
		viewerObject.transform.rotation.x = asin(-directionToOrigin.y); // pitch
		 
		auto currentTime = std::chrono::high_resolution_clock::now();

		InitialRenderSystem initialRenderSystem{ engineDevice, appRenderer.getSwapChainRenderPass() };
		KeyboardController controler{ initialRenderSystem };

		while (!appWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime =
				std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			controler.moveInPlaneXZ(appWindow.getGLFWwindow(), frameTime, viewerObject);
			controler.handleInput(appWindow.getGLFWwindow());
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = appRenderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);

			if (auto commandBuffer = appRenderer.beginFrame()) {
				int frameIndex = appRenderer.getCurrentFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };

				// Update memory
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToIndex(&ubo, frameIndex);
				uboBuffers[frameIndex]->flush();

				// Render
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				initialRenderSystem.renderGameObjects(frameInfo, gameObjects);
				appRenderer.endSwapChainRenderPass(commandBuffer);
				appRenderer.endFrame();

			}
		}

		vkDeviceWaitIdle(engineDevice.device());
	}

	std::unique_ptr<Model> createCubeModel(EngineDevice& device, glm::vec3 offset) {
		Model::ModelBuilder modelBuilder{};
		modelBuilder.vertices = {

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}
		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9, 
			12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<Model>(device, modelBuilder);
	}

	void VulkanApp::loadGameObjects()
	{
		std::shared_ptr<Model> model = Model::createModelFromFile(engineDevice, "models/smooth_vase.obj");
		auto flat_vase = GameObject::createGameObject();
		flat_vase.model = model;
		flat_vase.transform.translation = { 0.0f, .0f, 0.0f };
		flat_vase.transform.scale = { 1.f, 1.f, 1.f };
		gameObjects.push_back(std::move(flat_vase));
	}

}