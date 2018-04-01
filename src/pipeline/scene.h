#pragma once 

#include "object.h"
#include "shading.h"
#include "vertices.h"
#include "viewPort.h"
#include "windowBase.h"
#include "lighting.h"
#include "texture.h"
#include "viewPort.h"
#include "offscreen.h"
#include "testTextureObject.h"
#include <vector>

namespace bee
{

namespace scene_impl
{

class SceneBase
{
public:
	virtual ~SceneBase()
	{
		clear();
	}

	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value &&
			::std::is_base_of<Object, T>::value>::type>
	T &createObject(Types &&...args)
	{
		auto object = new T(::std::forward<Types>(args)...);
		object->scale(scaleFactor);
		objects.push_back(object);
		return *object;
	}
	void deleteObject(Object &object)
	{
		auto iter = ::std::find(objects.begin(), objects.end(), &object);
		if (iter != objects.end())
		{
			objects.erase(iter);
			delete &object;
		}
		else
		{
			BEE_RAISE(Fatal, "Cannot delete object.");
		}
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
	// void deleteController(gl::ShaderController &object)
	// {
	// 	auto iter = ::std::find(controllers.begin(), controllers.end(), &object);
	// 	if (iter != objects.end())
	// 	{
	// 		objects.erase(iter);
	// 		delete &object;
	// 	}
	// 	else
	// 	{
	// 		BEE_RAISE(Fatal, "Cannot delete object.");
	// 	}
	// }
	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value &&
			::std::is_base_of<ViewPort, T>::value>::type>
	T &createCamera(Types &&...args)
	{
		auto camera = new T(::std::forward<Types>(args)...);
		cameras.push_back(camera);
		return *camera;
	}
	void deleteCamera(ViewPort &camera)
	{
		auto iter = ::std::find(cameras.begin(), cameras.end(), &camera);
		if (iter != cameras.end())
		{
			cameras.erase(iter);
			delete &camera;
		}
		else
		{
			BEE_RAISE(Fatal, "Cannot delete camera.");
		}
	}
	void clear()
	{
		for (auto object: objects)
		{ delete object; }
		controllers.foreach([](gl::ShaderController *controller)
		{ delete controller; });
	}
	void setControllers() 
	{
		gl::ShaderControllers::setControllers(controllers);
	}
	void setScaleFactor(float e)
	{
		scaleFactor = e;
	}
protected:
	::std::vector<Object*> objects;
	::std::vector<ViewPort*> cameras;
	gl::ShaderControllers controllers;
private:
	float scaleFactor = 1;
};

class Scene final: public SceneBase
{
	BEE_UNIFORM_GLOB(int, ShadowMap);
	BEE_UNIFORM_GLOB(::glm::vec3, LightWorldPos);
	BEE_UNIFORM_GLOB(::glm::vec2, ShadowMapScale);
	BEE_UNIFORM_GLOB(float, ObjectIndex);
public:
	Scene()
	{
		majorLightCamera.setPerspectiveFov(::glm::radians(90.f));
		static int n = [this]()
		{
			Object::onSetViewMatrices([this](Object &self, ViewPort &camera)
			{
				gShadowMap = 0;
				gLightWorldPos = majorLight->getPosition();
				gShadowMapScale = ::glm::vec2(
					1.0 / WindowBase::getWidth(), 1.0 / WindowBase::getHeight());
			});
			return 0;
		}();
	}

	void renderPass()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		setControllers();
		shadowTexture.invoke(0);

		for (auto camera : cameras)
		{
			for (auto object: objects)
			{
				object->render(*camera);
			}
		}
	}
	void cursorPass()
	{
		cursorRenderBuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		gl::Shader::bind(*cursorShader);
		setControllers();

		int x = GLWindowBase::getCursorX();
		int y = GLWindowBase::getCursorY();

		for (auto iter = cameras.rbegin(); iter != cameras.rend(); ++iter)
		{
			int left = (*iter)->getLeft();
			int top = (*iter)->getTop();
			int width = (*iter)->getWidth();
			int height = (*iter)->getHeight();
			if (top <= y && left <= x && left + width> x && top + height > y)
			{	// max is 0x1fffffff
				for (unsigned i = 1; i <= objects.size(); ++i)
				{
					gObjectIndex = reinterpret_cast<float&>(i);
					objects[i - 1]->render(**iter);
				}
				break;
			}
		}

		int currentPos = 0;
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(x, WindowBase::getHeight() - 1 - y, 1, 1, 
			GL_RED, GL_FLOAT, &currentPos);
		// BEE_LOG(currentPos);
		
		auto &window = WindowBase::getInstance();
		int left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		int mouse = (left == GLFW_PRESS? 1 : 0) | (right == GLFW_PRESS ? 2 : 0);
		
		if (currentPos != 0)
		{
			mouseHover(objects[currentPos - 1]);
			if (mouse)
			{
				mouseClick(objects[currentPos - 1], mouse);
			}
		}
		else
		{
			mouseHover(nullptr);
			if (mouse)
			{
				mouseClick(nullptr, mouse);
			}
		}

		gl::Shader::unbind();
		
		cursorRenderBuffer.unbind();
	}
	void shadowPass()
	{
		glCullFace(GL_FRONT);
		glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		majorLightCamera.setPosition(majorLight->getPosition());
		
		gl::Shader::bind(*shadowShader);
		setControllers();
		
		for (int i = 0; i != 6; ++i)
		{
			shadowTexture.bind(i);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			shadowTexture.configureCamera(majorLightCamera);
			
			for (auto object: objects)
			{
				object->render(majorLightCamera);//majorLightCamera);
			}
		}
		shadowTexture.unbind();
		
		gl::Shader::unbind();
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glCullFace(GL_BACK);
	}
	void setMajorLight(const PointLight &light)
	{
		majorLight = &light;
	}
	void onMouseHover(const ::std::function<void(Object *)> &fn)
	{
		mouseHover = fn;
	}
	void onMouseClick(const ::std::function<void(Object *, int)> &fn)
	{
		mouseClick = fn;
	}
private:
	gl::Shader *shadowShader = &gl::Shader::load(
		"shadow",
		gl::VertexShader("shadow-vs.glsl"),
		gl::FragmentShader("shadow-fs.glsl")
	);
	gl::Shader *cursorShader = &gl::Shader::load(
		"cursor",
		gl::VertexShader("cursor-vs.glsl"),
		gl::FragmentShader("cursor-fs.glsl")
	);
private:
	gl::CubeDepthFBT shadowTexture;
	
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const PointLight *majorLight = nullptr;

	gl::SingleChannelFBRB cursorRenderBuffer;
	::std::function<void(Object *)> mouseHover;
	::std::function<void(Object *, int)> mouseClick;
};

}

using scene_impl::Scene;

}
