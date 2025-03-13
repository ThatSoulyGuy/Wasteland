#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Math/Vector.hpp"
#include "Utility/Exception/Exceptions/IllegalStateException.hpp"
#include "Utility/Exception/Exceptions/InvalidHandleException.hpp"

using namespace Wasteland::Math;
using namespace Wasteland::Utility::Exception::Exceptions;

namespace Wasteland::Core
{
	class Window final
	{

	public:
		
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

		void Initialize(const std::string& title, const Vector<int, 2>& dimensions)
		{
			if (!glfwInit())
				throw MAKE_EXCEPTION(IllegalStateException, "Failed to initialize GLFW!");

			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_VISIBLE, false);

			handle = glfwCreateWindow(dimensions.x(), dimensions.y(), title.c_str(), nullptr, nullptr);

			if (!handle)
				throw MAKE_EXCEPTION(InvalidHandleException, "Failed to create GLFW window handle!");

			glfwSetFramebufferSizeCallback(handle, [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });

			glfwMakeContextCurrent(handle);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				throw MAKE_EXCEPTION(IllegalStateException, "Failed to initialize GLAD!");

			glfwShowWindow(handle);
		}

		void Clear()
		{
			glClearColor(0.0f, 0.45f, 0.75f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Present()
		{
			glfwPollEvents();
			glfwSwapBuffers(handle);
		}
		
		std::string GetTitle() const
		{
			return glfwGetWindowTitle(handle);
		}

		void SetTitle(const std::string& title)
		{
			glfwSetWindowTitle(handle, title.c_str());
		}

		Vector<int, 2> GetDimensions() const
		{
			int width, height;

			glfwGetWindowSize(handle, &width, &height);

			return { width, height };
		}

		void SetDimensions(const Vector<int, 2>& dimensions)
		{
			glfwSetWindowSize(handle, dimensions.x(), dimensions.y());
		}

		Vector<int, 2> GetPosition() const
		{
			int x, y;

			glfwGetWindowPos(handle, &x, &y);

			return { x, y };
		}

		bool ShouldClose() const
		{
			return glfwWindowShouldClose(handle);
		}

		void Uninitialize()
		{
			glfwDestroyWindow(handle);
			
			glfwTerminate();
		}

		static Window& GetInstance()
		{
			std::call_once(initalizationFlag, [&]()
			{
				instance = std::unique_ptr<Window>(new Window());
			});

			return *instance;
		}

	private:

		Window() = default;

		GLFWwindow* handle;

		static std::once_flag initalizationFlag;
		static std::unique_ptr<Window> instance;

	};

	std::once_flag Window::initalizationFlag;
	std::unique_ptr<Window> Window::instance;
}