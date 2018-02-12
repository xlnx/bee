#pragma once

#include "windowBase.h"
#include "windowEvent.h"
#include "buffers.h"

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
	DropDispatcher
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<WindowBase, Types...>::value>::type >
	GLWindowBase(Types &&...args): 
		WindowBase(::std::forward<Types>(args)...)
	{
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
	}

	double getCursorX() const
	{
		return x;
	}
	double getCursorY() const
	{
		return y;
	}
	template <typename T>
	typename T::type *dispatch(const typename T::callbackType &callback, int zindex = 0)
	{
		using base = typename T::dispatcherType;
		return base::handlers->emplace(callback, zindex);
	}
protected:
	double x, y;
private:
	bool enter = true;
	gl::BufferGenerator bufferGenerator;
	gl::VertexArrayGenerator arrayGenerator;
};

template <int Major, int Minor>
class Window: public GLWindowBase
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<WindowBase, Types...>::value>::type >
	Window(Types &&...args): 
		GLWindowBase(::std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}
};

}