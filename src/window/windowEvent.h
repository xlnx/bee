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
	WindowEvent(listType &parent,
			const callbackType &callback, int priority = INT_MAX):
		callback(callback), parent(parent), priority(priority)
	{
	}
public:
	void cancel()
	{
		if (this->prev->next = this->next)
		{
			this->next->prev = this->prev;
		}
		else
		{
			if (this->prev->prev)
			{
				parent.endEvents = this->prev;
			}
			else
			{
				parent.endEvents = nullptr;
			}
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
	listType &parent;
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
		events(new eventType(*this, callbackType()))
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
		auto event = new eventType(*this, callback, priority), ptr = events;
		for (; ptr != nullptr; ptr = ptr->next)
		{
			if (ptr->priority < event->priority)
			{
				ptr->prev->next = event; event->prev = ptr->prev;
				event->next = ptr; ptr->prev = event; break;
			}
		}
		if (!ptr)
		{
			if (endEvents)
			{
				endEvents->next = event; event->prev = endEvents; endEvents = event;
			}
			else
			{
				events->next = event; event->prev = events; endEvents = event;
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
	eventType *events = nullptr, *endEvents = nullptr;
};

#define BEE_EVENT_DISPATCHER(eventName, ...) \
	struct eventName##Dispatcher\
	{\
	protected:\
		eventName##Dispatcher()\
		{\
			glfwSet##eventName##Callback(reinterpret_cast<WindowBase&>(*this), dispatch<__VA_ARGS__>);\
		}\
		template <typename ...Types>\
		static void dispatch(GLFWwindow *window, Types ...args)\
		{\
			for (auto &handler: handlers) \
			{\
				if (handler.callback(args...)) break;\
			}\
		}\
	protected:\
		static WindowEventList<__VA_ARGS__> handlers;\
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

}