#pragma once

#include "common.h"
#include "windowBase.h"
#include "property.h"
#include <glm/glm.hpp>
#include <utility>

namespace bee
{

class ViewPortBase
{
protected:
	ViewPortBase(::glm::vec3 target = {0, 1, 0},
			::glm::vec3 position = {0, 0, 0},
			::glm::vec3 up = {0, 0, 1})
	{
		setTarget(target); setPosition(position); setUp(up);
	}

	const ::glm::mat4 &getTrans()
	{
		if (modified())
		{
			if (rotateModified)
			{
				rotateModified = false;
				auto U = ::glm::normalize(::glm::cross(N, Up));
				auto V = ::glm::cross(U, N);
				rotateTrans = { 
					U.x, V.x, -N.x, 0.f, 
					U.y, V.y, -N.y, 0.f,
					U.z, V.z, -N.z, 0.f,
					0.f, 0.f, 0.f, 1.f
				};
			}
			if (translateModified)
			{
				translateModified = false;
				translateTrans = {
					1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					-P.x, -P.y, -P.z, 1.f
				};
			}
			trans = rotateTrans * translateTrans;
		}
		return trans;
	}
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setTarget(Types &&...args)
	{
		N = ::glm::normalize(::glm::vec3(::std::forward<Types>(args)...));
		rotateModified = true;
	}
	const ::glm::vec3 &getTarget() const
	{
		return N;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setUp(Types &&...args)
	{
		Up = ::glm::normalize(::glm::vec3(::std::forward<Types>(args)...));
		rotateModified = true;
	}
	const ::glm::vec3 &getUp() const
	{
		return Up;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setPosition(Types &&...args)
	{
		P = ::glm::vec3(::std::forward<Types>(args)...);
		translateModified = true;
	}
	const ::glm::vec3 &getPosition() const
	{
		return P;
	}
protected:
	bool modified() const
	{
		return rotateModified || translateModified;
	}
protected:
	bool rotateModified = true, translateModified = true;
	::glm::vec3 N, Up, P;
private:
	::glm::mat4 rotateTrans, translateTrans, trans;
}; 

class Viewport: public ViewPortBase
{
public:
	Viewport() = default;
	Viewport(GLint left, GLint top, GLint width, GLint height):
		fLeft(left), fTop(top), fWidth(width), fHeight(height)
	{
	}
	
	const ::glm::mat4 &getTrans()
	{
		if (activePort() != this)
		{
			activePort() = this;
#			ifdef __APPLE__
			int w, h;
			glfwGetFramebufferSize(WindowBase::getInstance(), &w, &h);
			glViewport(fLeft, fTop, w, h);
#			endif
		}
		if (modified())
		{
			if (perspectiveModified)
			{
				perspectiveModified = false;
				perspectiveTrans = ::glm::perspective(fov, float(fWidth) / fHeight, zNear, zFar);
			}
			trans = perspectiveTrans * ViewPortBase::getTrans();
		}
		return trans;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setPerspective(Types &&...args)
	{
		::glm::vec3 v = {args...};
		fov = v[0]; zNear = v[1]; zFar = v[2];
		perspectiveModified = true;
	}
	void setPerspectiveFov(GLfloat radians)
	{
		fov = radians;
		perspectiveModified = true;
	}
	void setPerspectiveZNear(GLfloat z)
	{
		zNear = z;
		perspectiveModified = true;
	}
	void setPerspectiveZFar(GLfloat z)
	{
		zFar = z;
		perspectiveModified = true;
	}
protected:
	bool modified() const
	{
		return perspectiveModified || ViewPortBase::modified();
	}
	static Viewport *&activePort()
	{
		static Viewport *port = nullptr;
		return port;
	}
private:
	GLfloat fov = ::glm::radians(60.f), zNear = 1e-3f, zFar = 1e5f;//100.f;
	BEE_PROPERTY_R(GLint, Left) = 0;
	BEE_PROPERTY_R(GLint, Top) = 0;
	BEE_PROPERTY_R(GLint, Width) = WindowBase::getWidth();
	BEE_PROPERTY_R(GLint, Height) = WindowBase::getHeight();
	bool perspectiveModified = true;
	::glm::mat4 perspectiveTrans, trans;
};

}