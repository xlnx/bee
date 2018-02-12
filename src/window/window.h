#pragma once

#include "windowBase.h"
#include "windowEvent.h"

namespace bee
{

template <int Major, int Minor>
class Window: public WindowBase,
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
	Window(Types &&...args): 
		WindowBase(::std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}

	template <typename T>
	typename T::type *dispatch(const typename T::callbackType &callback, int priority = 0)
	{
		using base = typename T::dispatcherType;
		return base::handlers.emplace(callback, priority);
	}
};

}