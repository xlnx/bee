#pragma once 

#include "object.h"
#include "shading.h"
#include "vertices.h"
#include "viewPort.h"
#include "windowBase.h"
#include "lighting.h"
#include "texture.h"
#include "viewPort.h"
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
		return texture.invoke(i);
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
public:
	Scene()
	{
		majorLightCamera.setPerspectiveFov(90.f);
		static int n = [this]()
		{
			Object::onSetViewMatrices([this](Object &self, ViewPort &camera)
			{
				gShadowMap = 0;
				gLightWorldPos = majorLight->getPosition();
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

		for (auto camera: cameras)
		{
			for (auto object: objects)
			{
				object->render(*camera);
			}
		}

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
			// majorLightCamera.setTarget(0, 0, -1);
			// majorLightCamera.setUp(1, 0, 0);
			
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
	ShadowFramebuffer shadowFramebuffer;
	ViewPort majorLightCamera = ViewPort(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const PointLight *majorLight = nullptr;
};

}
