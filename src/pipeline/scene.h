#pragma once 

#include "object.h"
#include "shading.h"
#include "vertices.h"
#include "viewport.h"
#include "windowBase.h"
#include "lighting.h"
#include "texture.h"
#include "viewport.h"
#include "offscreen.h"

#include "sceneWidgets.h"
#include "widgets.h"

#include "testTextureObject.h"
#include <vector>
#include <list>

namespace bee
{

namespace scene_impl
{

class Scene;
class SceneBase;

template <typename T>
class SceneWrapper: public T
{
	friend class Scene;
	friend class SceneBase;
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value>::type>
	SceneWrapper(Types &&...args):
		T(::std::forward<Types>(args)...)
	{
	}
public:
	void translate(float dx, float dy, float dz) override;
	void translate(const ::glm::vec3 &diff) override;
	void setPosition(float x, float y, float z) override;
	void setPosition(const ::glm::vec3 &pos) override;
private:
	SceneBase *scene = nullptr;
};

class SceneBase
{
	// template <typename>
private:
	template <bool X, typename A, typename B>
	struct SwitchType
	{
		using type = A;
	};
	template<typename A, typename B>
	struct SwitchType<false, A, B>
	{
		using type = B;
	};
	template <typename T>
	struct CreateType
	{
		using type = typename SwitchType<
			::std::is_base_of<Object, T>::value,
			Object,
			typename SwitchType<
				::std::is_base_of<gl::ShaderController, T>::value,
				gl::ShaderController,
				Viewport
			>::type
		>::type;
	};
	template <typename BaseType>
	struct GetListElemType
	{
		using type = typename SwitchType<
			::std::is_same<BaseType, Object>::value,
			::std::pair<BaseType*, int>,
			BaseType*
		>::type;
	};
	template <typename T>
	struct GenType
	{
		using type = typename SwitchType<
			::std::is_base_of<Object, T>::value,
			SceneWrapper<T>,
			T
		>::type;
	};
public:
	template <typename T>
	class Ref
	{
		friend class SceneBase;
		using ListElemType = typename GetListElemType<
			typename CreateType<T>::type>::type;
		static constexpr auto ElemIsObject = ::std::is_same<
			ListElemType, ::std::pair<
				typename CreateType<T>::type*, int>
		>::value;
	public:
		Ref() = default;
		Ref(const typename ::std::list<ListElemType>::iterator &iter):
			iter(iter), valid(true)
		{
		}

		bool isValid()
		{
			return this->valid;
		}
		void invalidate()
		{
			valid = false;
		}
		T *operator -> ()
		{
			if constexpr (ElemIsObject)
			{
				return static_cast<T*>(iter->first);
			}
			else
			{
				return static_cast<T*>(*iter);
			}
		}
		T &operator * ()
		{
			if constexpr (ElemIsObject)
			{
				return static_cast<T&>(*iter->first);
			}
			else
			{
				return static_cast<T&>(**iter);
			}
		}
		operator T *()
		{
			if constexpr (ElemIsObject)
			{
				return static_cast<T*>(iter->first);
			}
			else
			{
				return static_cast<T*>(*iter);
			}
		}
	private:
		typename ::std::list<ListElemType>::iterator iter;
		bool valid = false;
	};
public:
	virtual ~SceneBase()
	{
		clear();
	}

	template <typename T, typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<T, Types...>::value && 
				::std::is_base_of<Object, T>::value ||
				::std::is_base_of<gl::ShaderController, T>::value || 
				::std::is_base_of<Viewport, T>::value
			>::type>
	Ref<T> create(Types &&...args)
	{
		auto t = new typename GenType<T>::type(::std::forward<Types>(args)...);
		if constexpr (::std::is_base_of<Object, T>::value)
		{
			t->scene = this;
			auto iter = objects.insert(objects.end(), ::std::make_pair(t, int(index.size())));
			t->scale(scaleFactor); 
			index.push_back(iter);
			return Ref<T>(iter);
		}
		if constexpr (::std::is_base_of<gl::ShaderController, T>::value)
		{
			return Ref<T>(controllers.addController(*t));
		}
		if constexpr (::std::is_base_of<Viewport, T>::value)
		{
			return Ref<T>(cameras.insert(cameras.end(), t));
		}
	}

	template <typename T, typename = typename
		::std::enable_if<
				::std::is_base_of<Object, T>::value ||
				::std::is_base_of<gl::ShaderController, T>::value || 
				::std::is_base_of<Viewport, T>::value
			>::type>
	void destroy(Ref<T> &elem)
	{
		if constexpr (::std::is_base_of<Object, T>::value)
		{
			auto back = objects.end(); back--;
			::std::swap(index[elem.iter->second], index.back());
			::std::swap(*elem.iter, *back);
			delete static_cast<Object*>(elem); objects.erase(back);
		}
		if constexpr (::std::is_base_of<gl::ShaderController, T>::value)
		{
			delete static_cast<gl::ShaderController*>(elem); controllers.removeController(elem.iter);
		}
		if constexpr (::std::is_base_of<Viewport, T>::value)
		{
			delete static_cast<Viewport*>(elem); cameras.erase(elem.iter);
		}
	}
	void clear()
	{
		for (auto object: objects)
		{
			delete object.first;
		}
		controllers.foreach(
			[](gl::ShaderController *controller)
			{
				delete controller; 
			}
		);
	}
	void setControllers() 
	{
		gl::ShaderControllers::setControllers(controllers);
	}
	void setScaleFactor(float e)
	{
		scaleFactor = e;
	}
	template <typename T>
	void translate(SceneWrapper<T> &object, float dx, float dy, float dz)
	{
		object.T::translate(dx * scaleFactor, dy * scaleFactor, dz * scaleFactor);
	}
	template <typename T>
	void translate(SceneWrapper<T> &object, const ::glm::vec3 &diff)
	{
		object.T::translate(diff * scaleFactor);
	}
	template <typename T>
	void setPosition(SceneWrapper<T> &object, float x, float y, float z)
	{
		object.T::setPosition(x * scaleFactor, y * scaleFactor, z * scaleFactor);
	}
	template <typename T>
	void setPosition(SceneWrapper<T> &object, const ::glm::vec3 &pos)
	{
		object.T::setPosition(pos * scaleFactor);
	}
protected:
	::std::list<::std::pair<Object*, int>> objects;
	::std::vector<typename ::std::list<
		::std::pair<Object*, int>>::iterator> index;
	::std::list<Viewport*> cameras;
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
	Scene(float scaleFactor = 1.f)
	{
		setScaleFactor(scaleFactor);
		majorLightCamera.setPerspectiveFov(::glm::radians(90.f));
		static int n = [this]()
		{
			Object::onSetViewMatrices([this](Object &self, Viewport &camera)
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
				object.first->render(*camera);
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
					for (auto object: objects)
					{
						auto idx = object.second + 1;
						gObjectIndex = reinterpret_cast<float&>(idx);
						object.first->render(**iter);
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
			
			SelectUtil *obj;
			if (currentPos != 0 && (obj = dynamic_cast<SelectUtil*>(index[currentPos - 1]->first)))
			{
				if (hoverObject != obj)
				{
					if (hoverObject)
					{
						hoverObject->hover(false);
					}
					obj->hover(true);
					hoverObject = obj;
				}
				if (mouse)
				{
					if (selectedObject != obj)
					{
						if (selectedObject)
						{
							selectedObject->select(false);
						}
						obj->select(true);
						selectedObject = obj;
					}
				}
				// if (!multiSelection)
				// {
				// 	sele
				// }
			}
			else
			{
				if (hoverObject)
				{
					hoverObject->hover(false);
					hoverObject = nullptr;
				}
				if (mouse && selectedObject)
				{
					selectedObject->select(false);
					selectedObject = nullptr;
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
					object.first->render(majorLightCamera);//majorLightCamera);
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
	gl::Shader *cursorShader = &gl::Shader::load(
		"cursor",
		gl::VertexShader("cursor-vs.glsl"),
		gl::FragmentShader("cursor-fs.glsl")
	);
private:
	gl::CubeDepthFBT shadowTexture;
	
	Viewport majorLightCamera = Viewport(0, 0, 
		GLWindowBase::getWidth(), GLWindowBase::getHeight());
	const PointLight *majorLight = nullptr;

	gl::SingleChannelFBRB cursorRenderBuffer;

	SelectUtil *hoverObject = nullptr;
	SelectUtil *selectedObject = nullptr;
};

template <typename T>
void SceneWrapper<T>::translate(float dx, float dy, float dz) 
{
	scene->translate(*this, dx, dy, dz);
}
template <typename T>
void SceneWrapper<T>::translate(const ::glm::vec3 &diff) 
{
	scene->translate(*this, diff);
}
template <typename T>
void SceneWrapper<T>::setPosition(float x, float y, float z) 
{
	scene->setPosition(*this, x, y, z);
}
template <typename T>
void SceneWrapper<T>::setPosition(const ::glm::vec3 &pos) 
{
	scene->setPosition(*this, pos);
}

}

using scene_impl::Scene;

}
