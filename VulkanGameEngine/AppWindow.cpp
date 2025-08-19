#include "AppWindow.h"

namespace app{
	AppWindow::AppWindow(int w, int h, std::string name) : width{w}, height{h}, windowname{name}
	{
		initWindow();
	}

	AppWindow::~AppWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool AppWindow::shouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void AppWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void AppWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto appWindow = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
		appWindow->frameBufferResized = true;
		appWindow->width = width;
		appWindow->height = height;
	}

	void AppWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
}

