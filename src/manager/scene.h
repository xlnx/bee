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
				gLightWVP = ::glm::transpose(majorLightCamera.getTrans() * self.getTrans());//majorLight->getViewMatrix();
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

		// gl::Shader::bind(*shadowShader);
		// for (auto camera: cameras)
		// {
		// 	static MeshObject mesh = MeshObject(
		// 		*new gl::Mesh(gl::VertexAttrs<gl::pos3, gl::tex2>{
		// 			{{0, -1, 1}, {0, 1}},
		// 			{{0, 1, 1}, {1, 1}},
		// 			{{0, 1, -1}, {1, 0}},
		// 			{{0, -1, -1}, {0, 0}}
		// 		}, gl::Faces{
		// 			{0, 1, 2},
		// 			{2, 3, 0}
		// 		})
		// 	);
		// 	mesh.render(*cameras[0]);
		// }
		// gl::Shader::unbind();
	}
	void renderDepth()
	{
		depthFramebuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		gl::ShaderControllers::setControllers(controllers);
		majorLightCamera.setPosition(0, 0, 2);
		majorLightCamera.setTarget(0, 0, -1);
		majorLightCamera.setUp(1, 0, 0);
		gl::Shader::bind(*shadowShader);
		for (auto object: objects)
		{
			object->render(majorLightCamera);//majorLightCamera);
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
	void setMajorLight(const PointLight &light)
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
	const PointLight *majorLight = nullptr;
};

}
