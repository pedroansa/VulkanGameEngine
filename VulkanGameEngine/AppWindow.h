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
		GLFWwindow* getGLFWwindow() const { return window; }

		bool shouldClose();
		bool wasWindowResized() { return frameBufferResized; };
		void resetWindowResizedFlag() { frameBufferResized = false; };
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();
		int width, height;
		bool frameBufferResized = false;
		
		std::string windowname;
		GLFWwindow* window;
	};
}


