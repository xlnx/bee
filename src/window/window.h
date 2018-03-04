#pragma once

#include "windowBase.h"
#include "windowEvent.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

namespace bee
{

class GLWindowBase;

class WindowController
{
	friend class GLWindowBase;
protected:
	WindowController() = default;
public:
	virtual ~WindowController() = default;
};

class GLWindowBase: public WindowBase,
//  GLFW event dispatchers
	KeyDispatcher,
	CharDispatcher,
	CharModsDispatcher,
	CursorPosDispatcher,
	CursorEnterDispatcher,
	MouseButtonDispatcher,
	ScrollDispatcher,
	// JoystickDispatcher,
	DropDispatcher,
	RenderDispatcher,
	UpdateDispatcher
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<WindowBase, Types...>::value>::type >
	GLWindowBase(Types &&...args): 
		WindowBase(::std::forward<Types>(args)...)
	{
		init();
		gl::Shader::initialize();
		dispatch<CursorPosEvent>(
			[this](double x, double y)->bool {
				this->x = x; this->y = y; return false;
			}, INT_MIN
		);
		dispatch<CursorEnterEvent>(
			[this](int enter)->bool {
				if (!enter) this->enter = true;
				return false;
			}, INT_MAX
		);
		dispatch<CursorPosEvent>(
			[this](double x, double y)->bool {
				if (this->enter) {
					this->x = x; this->y = y; this->enter = false;
				} return false;
			}, INT_MAX
		);
		dispatch<RenderEvent>(
			[this]() -> bool {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				int time0 = glfwGetTime();
				if (time0 - timeStamp == 1)
				{
					frameCount = 0;
				}
				else
				{
					frameCount++;
				}
				timeStamp = time0;
				return false;
			}, INT_MAX
		);
	}
	~GLWindowBase()
	{
		deleteControllers();
	}

	static double getCursorX()
	{
		return static_cast<GLWindowBase*>(instance)->x;
	}
	static double getCursorY()
	{
		return static_cast<GLWindowBase*>(instance)->y;
	}
	template <typename T>
	static typename T::type &dispatch(const typename T::callbackType &callback, int zindex = 0)
	{
		using base = typename T::dispatcherType;
		return base::handlers->emplace(callback, zindex);
	}
	void dispatchMessages()
	{
		static double millis = glfwGetTime();
		auto curr = millis;
		while (!closed())
		{
			UpdateDispatcher::dispatch((curr - millis) * 1000);
			RenderDispatcher::dispatch();
			gl::checkError(); swapBuffers(); pollEvents();
			millis = curr; curr = glfwGetTime();
		}
	}
	// template <typename T, typename ...Types, typename = typename
	// 	::std::enable_if<::std::is_constructible<T, GLWindowBase &, Types...>::value &&
	// 		::std::is_base_of<WindowController, T>::value>::type>
	// T &createController(Types &&...args)
	// {
	// 	auto controller = new T(*this, ::std::forward<Types>(args)...);
	// 	controllers.push_back(controller);
	// 	return *controller;
	// }
	void deleteControllers()
	{
		for (auto controller: controllers)
		{
			delete controller;
		}
	}
private:
	static void init()
	{
		static gl::BufferGenerator bufferGenerator;
		static gl::VertexArrayGenerator arrayGenerator;
		static gl::TextureGenerator textureGenerator;
	}
protected:
	double x = -1, y = -1;
	int frameCount = 0, timeStamp = 0;
	::std::vector<WindowController*> controllers;
private:
	bool enter = true;
};

template <int Major, int Minor>
class Window: public GLWindowBase
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<GLWindowBase, Types...>::value>::type >
	Window(Types &&...args): 
		GLWindowBase(::std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		gl::Shader::setFilePath("shaders/");
		gl::Model::setFilePath("models/");
		gl::Material::setFilePath("materials/");
	}
};

}
