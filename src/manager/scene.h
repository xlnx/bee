#pragma once 

#include "object.h"
#include "shading.h"
#include "bufferControllers.h"
#include "viewPort.h"
#include "windowBase.h"
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
			::std::is_base_of<gl::ShaderController, T>::value>::type>
	T &createController(Types &&...args)
	{
		auto controller = new T(::std::forward<Types>(args)...);
		controllers.addController(*controller);
		return *controller;
	}
	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value &&
			::std::is_base_of<ViewPort, T>::value>::type>
	T &createCamera(Types &&...args)
	{
		auto camera = new T(::std::forward<Types>(args)...);
		cameras.push_back(camera);
		return *camera;
	}
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		depthFramebuffer.invoke(0);
		for (auto camera: cameras)
		{
			for (auto object: objects)
			{
				gl::ShaderControllers::setControllers(controllers);
				object->render(*camera);
			}
		}
	}
	void renderDepth()
	{
		depthFramebuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		for (auto object: objects)
		{
			gl::ShaderControllers::setControllers(controllers);
			object->render(majorLightCamera);
		}
		depthFramebuffer.unbind();
	}
	void clear()
	{
		for (auto object: objects)
		{
			delete object;
		}
		controllers.foreach([](gl::ShaderController *controller)
		{
			delete controller;
		});
	}
private:
	::std::vector<Object*> objects;
	::std::vector<ViewPort*> cameras;
	gl::ShaderControllers controllers;
	gl::DepthFramebuffer depthFramebuffer;
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
};

}
