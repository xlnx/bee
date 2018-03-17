#pragma once 

#include "object.h"
#include "shading.h"
#include "vertices.h"
#include "viewPort.h"
#include "windowBase.h"
#include "lighting.h"
#include "texture.h"
#include "viewPort.h"
#include "testTextureObject.h"
#include <vector>

namespace bee
{

class ShadowFramebuffer
{
public:
	ShadowFramebuffer()
	{
		fbo.bind();
			texture2D.attachTo(fbo);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			fbo.validate();
		fbo.unbind();
	}
	void bind(int i)
	{
		fbo.bind();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		cubeface = i;
	}
	void unbind()
	{
		fbo.unbind();
	}
	void invoke(int i) const
	{
		texture.invoke(i);
	}
	void configureCamera(ViewPort &viewPort)
	{
		static const ::glm::vec3 cameraInfos[][2] = {
			{ {1.f, 0.f, 0.f},  {0.f, -1.f, 0.f} },
			{ {-1.f, 0.f, 0.f}, {0.f, -1.f, 0.f} },
			{ {0.f, 1.f, 0.f},  {0.f, 0.f, 1.f} },
			{ {0.f, -1.f, 0.f}, {0.f, 0.f, -1.f} },
			{ {0.f, 0.f, 1.f},  {0.f, -1.f, 0.f} },
			{ {0.f, 0.f, -1.f}, {0.f, -1.f, 0.f} }
		};
		viewPort.setTarget(cameraInfos[cubeface][0]);
		viewPort.setUp(cameraInfos[cubeface][1]);
	}
	const gl::CubeDepthTexture &getTexture() const 
	{
		return texture;
	}
private:
	gl::FBO fbo;
	gl::DepthTexture texture2D;
	gl::CubeDepthTexture texture;
	int cubeface = 0;
};

class Scene
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
		shadowFramebuffer.invoke(0);

		for (auto camera : cameras)
		{
			for (auto object: objects)
			{
				object->render(*camera);
			}
		}
		
		// static TestTexture3DObject test3D(shadowFramebuffer.getTexture());
		// test3D.setTarget(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		// for (auto camera: cameras)
		// {
		// 	test3D.render(*camera);
		// }
	}
	void renderDepth()
	{
		glCullFace(GL_FRONT);
		glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		majorLightCamera.setPosition(majorLight->getPosition());
		
		gl::Shader::bind(*shadowShader);
		
		gl::ShaderControllers::setControllers(controllers);
		for (int i = 0; i != 6; ++i)
		{
			shadowFramebuffer.bind(i);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			shadowFramebuffer.configureCamera(majorLightCamera);
			
			for (auto object: objects)
			{
				object->render(majorLightCamera);//majorLightCamera);
			}
		}
		shadowFramebuffer.unbind();
		
		gl::Shader::unbind();
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glCullFace(GL_BACK);
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
	gl::Shader *shadowShader = &gl::Shader::load(
		"shadow",
		gl::VertexShader("shadow-vs.glsl"),
		gl::FragmentShader("shadow-fs.glsl")
	);
private:
	::std::vector<Object*> objects;
	::std::vector<ViewPort*> cameras;
	gl::ShaderControllers controllers;
	ShadowFramebuffer shadowFramebuffer;
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const PointLight *majorLight = nullptr;
};

}
