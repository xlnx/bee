#pragma once 

#include "object.h"
#include "shading.h"
#include <vector>

namespace bee
{

class Scene
{
public:
	Scene() = default;
	virtual ~Scene()
	{
		clear();
	}

	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value &&
			::std::is_base_of<Object, T>::value>::type>
	T &createObject(Types &&...args)
	{
		auto object = new T(::std::forward<Types>(args)...);
		objects.push_back(object);
		return *object;
	}
	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value &&
			::std::is_base_of<ShaderController, T>::value>::type>
	T &createController(Types &&...args)
	{
		auto controller = new T(::std::forward<Types>(args)...);
		controllers.addController(*controller);
		return *controller;
	}
	void render(ViewPort &camera)
	{
		for (auto object: objects)
		{
			object->render(camera);
			controllers.invoke();
		}
	}
	void clear()
	{
		for (auto object: objects)
		{
			delete object;
		}
		controllers.foreach([](ShaderController *controller)
		{
			delete controller;
		});
	}
private:
	::std::vector<Object*> objects;
	ShaderControllers<> controllers;
};

}
