#pragma once

#include "windowBase.h"
#include "windowEvent.h"
#include "buffers.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

namespace bee
{

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
	RenderDispatcher
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<WindowBase, Types...>::value>::type >
	GLWindowBase(Types &&...args): 
		WindowBase(::std::forward<Types>(args)...)
	{
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

	static double getCursorX()
	{
		return static_cast<GLWindowBase*>(instance)->x;
	}
	static double getCursorY()
	{
		return static_cast<GLWindowBase*>(instance)->y;
	}
	template <typename T>
	static typename T::type *dispatch(const typename T::callbackType &callback, int zindex = 0)
	{
		using base = typename T::dispatcherType;
		return base::handlers->emplace(callback, zindex);
	}
	void dispatchMessages()
	{
		while (!closed())
		{
			RenderDispatcher::dispatch();
			gl::checkError(); swapBuffers(); pollEvents();
		}
	}
protected:
	double x = -1, y = -1;
	int frameCount = 0, timeStamp = 0;
private:
	bool enter = true;
	gl::BufferGenerator bufferGenerator;
	gl::VertexArrayGenerator arrayGenerator;
	gl::TextureGenerator textureGenerator;
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

		gl::Shader::setFilePath("shaders/");
		gl::Model::setFilePath("models/");
	}
};

}