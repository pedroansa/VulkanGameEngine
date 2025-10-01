#pragma once
#include "AppWindow.h"
#include "EngineDevice.h"
#include "AppRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "InitialRenderSystem.h"
#include "KeyboardController.h"
#include "AppBuffer.h"
#include "FrameInfo.h"

#include <chrono>
#include <memory>
#include <vector>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace app {
	class VulkanApp
	{

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		VulkanApp();
		~VulkanApp();

		VulkanApp(const VulkanApp&) = delete;
		VulkanApp& operator=(const VulkanApp&) = delete;

		void run();

	private:
		void loadGameObjects();
		void sierpinski(std::vector<Model::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

		AppWindow appWindow{ WIDTH, HEIGHT, "Vulkan Game Engine" };
		EngineDevice engineDevice{ appWindow };
		AppRenderer appRenderer{ appWindow, engineDevice };

		std::vector<GameObject> gameObjects;
	};
}


