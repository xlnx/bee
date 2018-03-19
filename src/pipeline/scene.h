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
	void clear()
	{
		for (auto object: objects)
		{ delete object; }
		controllers.foreach([](gl::ShaderController *controller)
		{ delete controller; });
	}
protected:
	::std::vector<Object*> objects;
	::std::vector<ViewPort*> cameras;
	gl::ShaderControllers controllers;
};

class Scene final: public SceneBase
{
	BEE_UNIFORM_GLOB(int, ShadowMap);
	BEE_UNIFORM_GLOB(::glm::vec3, LightWorldPos);
	BEE_UNIFORM_GLOB(::glm::vec2, ShadowMapScale);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl::ShaderControllers::setControllers(controllers);
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
		// gl::Shader::bind(*cursorShader);

		// gl::ShaderControllers::setControllers(controllers);
		// for (auto object: objects)
		// {
		// 	object->render(majorCamera);
		// }

		// gl::Shader::unbind();
	}
	void shadowPass()
	{
		glCullFace(GL_FRONT);
		glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		majorLightCamera.setPosition(majorLight->getPosition());
		
		gl::Shader::bind(*shadowShader);
		
		gl::ShaderControllers::setControllers(controllers);
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
private:
	gl::Shader *shadowShader = &gl::Shader::load(
		"shadow",
		gl::VertexShader("shadow-vs.glsl"),
		gl::FragmentShader("shadow-fs.glsl")
	);
private:
	gl::CubeDepthFBT shadowTexture;
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const PointLight *majorLight = nullptr;
};

}

using scene_impl::Scene;

}
