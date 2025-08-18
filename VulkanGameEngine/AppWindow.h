#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <string>
#include <stdexcept>

namespace app {
	class AppWindow
	{
	public:
		AppWindow(int w, int h, std::string name);
		~AppWindow();

		AppWindow(const AppWindow&) = delete;
		AppWindow& operator=(const AppWindow&) = delete;

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; };

		bool shouldClose();
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void initWindow();
		const int width, height;
		
		std::string windowname;
		GLFWwindow* window;
	};
}


