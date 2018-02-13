#pragma once

#include "common.h"
#include "windowBase.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <climits>

namespace bee
{

template <typename ...Types>
class WindowEventList;

template <typename ...Types>
class WindowEvent;

template <typename ...Types>
class WindowEvent final
{
	friend class WindowEventList<Types...>;
public:
	using callbackType = ::std::function<bool(Types...)>;
	using listType = WindowEventList<Types...>;
private:
	WindowEvent(const callbackType &callback, int priority = INT_MAX):
		callback(callback), priority(priority)
	{
	}
public:
	void cancel()
	{
		if (this->prev->next = this->next)
		{
			this->next->prev = this->prev;
		}
		delete this;
	}
	bool invoke(Types &&...args) const
	{
		return callback(::std::forward<Types>(args)...);
	}
	int getPriority() const
	{
		return priority;
	}
public:
	const callbackType callback;
private:
	WindowEvent *prev = nullptr, *next = nullptr;
	const int priority;
};

template <typename ...Types>
class WindowEventList final
{
	friend class WindowEvent<Types...>;
public:
	using eventType = WindowEvent<Types...>;
	using callbackType = ::std::function<bool(Types...)>;

	class constIterator
	{
	public:
		constIterator(const eventType *ptr):
			ptr(ptr)
		{
		}
		constIterator &operator ++ ()
		{
			ptr = ptr->next; return *this;
		}
		bool operator != (const constIterator &other) const
		{
			return ptr != other.ptr;
		}
		const eventType &operator *() const
		{
			return *ptr;
		}
	private:
		const eventType *ptr;
	};

	WindowEventList():
		events(new eventType(callbackType()))
	{
	}
	~WindowEventList()
	{
		for (auto ptr = events, p = ptr->next; ptr != nullptr; ptr = p)
		{
			p = ptr->next; delete ptr;
		}
	}

	eventType *emplace(const callbackType &callback, int priority = 0)
	{
		auto event = new eventType(callback, priority), ptr = events;
		for (; ptr->next != nullptr; ptr = ptr->next)
		{
			if (ptr->priority < event->priority)
			{
				ptr->prev->next = event; event->prev = ptr->prev;
				event->next = ptr; ptr->prev = event; break;
			}
		}
		if (ptr->next == nullptr)
		{
			if (ptr->priority < event->priority)
			{
				ptr->prev->next = event; event->prev = ptr->prev;
				event->next = ptr; ptr->prev = event;
			}
			else
			{
				ptr->next = event; event->prev = ptr;
			}
		}
		return event;
	}
	constIterator begin() const
	{
		return constIterator(events->next);
	}
	constIterator end() const
	{
		return constIterator(nullptr);
	}
private:
	eventType *events = nullptr;
};

#define BEE_EVENT_DISPATCHER(eventName, ...) \
	struct eventName##Dispatcher\
	{\
	protected:\
		eventName##Dispatcher()\
		{\
			handlers = new WindowEventList<__VA_ARGS__>;\
			glfwSet##eventName##Callback(reinterpret_cast<WindowBase&>(*this), dispatch<__VA_ARGS__>);\
		}\
		template <typename ...Types>\
		static void dispatch(GLFWwindow *window, Types ...args)\
		{\
			for (auto &handler: *handlers) \
			{\
				if (handler.callback(args...)) break;\
			}\
		}\
	protected:\
		static WindowEventList<__VA_ARGS__> *handlers;\
	};\
	struct eventName##Event\
	{\
		using type = WindowEvent<__VA_ARGS__>;\
		using callbackType = typename WindowEventList<__VA_ARGS__>::callbackType;\
		using dispatcherType = eventName##Dispatcher;\
	};

BEE_EVENT_DISPATCHER(Key, int, int, int, int);

BEE_EVENT_DISPATCHER(Char, unsigned int);

BEE_EVENT_DISPATCHER(CharMods, unsigned int, int);

BEE_EVENT_DISPATCHER(CursorPos, double, double);

BEE_EVENT_DISPATCHER(CursorEnter, int);

BEE_EVENT_DISPATCHER(MouseButton, int, int, int);

BEE_EVENT_DISPATCHER(Scroll, double, double);

// BEE_EVENT_DISPATCHER(Joystick, int, int);

BEE_EVENT_DISPATCHER(Drop, int, const char**);

struct RenderDispatcher
{
protected:
	RenderDispatcher()
	{
		handlers = new WindowEventList<>;
	}
	static void dispatch()
	{
		for (auto &handler: *handlers)
		{
			if (handler.callback()) break;
		}
	}
protected:
	static WindowEventList<> *handlers;
};
struct RenderEvent
{
	using type = WindowEvent<>;
	using callbackType = typename WindowEventList<>::callbackType;
	using dispatcherType = RenderDispatcher;
};

}