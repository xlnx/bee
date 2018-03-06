#pragma once 

#include "object.h"
#include "shading.h"
#include "bufferControllers.h"
#include "viewPort.h"
#include "windowBase.h"
#include "lighting.h"
#include <vector>

namespace bee
{

class Scene
{
	BEE_UNIFORM_GLOB(int, ShadowMap);
	BEE_UNIFORM_GLOB(::glm::mat4, LightWVP);
public:
	Scene()
	{
		static int n = [this]()
		{
			Object::onSetViewMatrices([this](Object &self, ViewPort &camera)
			{
				gShadowMap = 0;
				gLightWVP = ::glm::transpose(camera.getTrans() * self.getTrans());//majorLight->getViewMatrix();
			});
			return 0;
		}();
	}
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
		gl::ShaderControllers::setControllers(controllers);
		depthFramebuffer.invoke(0);
		for (auto camera: cameras)
		{
			for (auto object: objects)
			{
				object->render(*camera);
			}
		}
	}
	void renderDepth()
	{
		depthFramebuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		gl::ShaderControllers::setControllers(controllers);
		// if (auto direct = dynamic_cast<DirectionalLight*>(majorLight))
		// {
		// 	majorLightCamera.setPosition(majorLight->getPosition);

		// }
		// else
		// {

		// }
		gl::Shader::bind(*shadowShader);
		for (auto object: objects)
		{
			object->render(*cameras[0]);//majorLightCamera);
		}
		gl::Shader::unbind();
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
	void setMajorLight(const LightBase &light)
	{
		majorLight = &light;
	}
private:
	static gl::Shader *getShadowShader()
	{
		static auto var = new gl::Shader(
			gl::VertexShader("shadow-vs.glsl"),
			gl::FragmentShader("shadow-fs.glsl")
		);
		return var;
	}
private:
	gl::Shader *shadowShader = getShadowShader();
private:
	::std::vector<Object*> objects;
	::std::vector<ViewPort*> cameras;
	gl::ShaderControllers controllers;
	gl::DepthFramebuffer depthFramebuffer;
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const LightBase *majorLight = nullptr;
};

}
