#pragma once

#include "common.h"
#include "camera.h"
#include "window.h"

namespace bee
{

class ViewPort: public Camera
{
public:
	ViewPort() = default;
	ViewPort(GLint left, GLint top, GLint width, GLint height):
		left(left), top(top), width(width), height(height)
	{
	}
	
	const ::glm::mat4 &getTrans()
	{
		if (activePort != this)
		{
			activePort = this;
			glViewport(left, top, width, height);
		}
		if (modified())
		{
			if (perspectiveModified)
			{
				perspectiveModified = false;
				perspectiveTrans = ::glm::perspective(::glm::radians(fov), 
					float(WindowBase::getWidth()) / WindowBase::getHeight(), zNear, zFar);
			}
			trans = perspectiveTrans * Camera::getTrans();
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
	void setPerspectiveFov(GLfloat degree)
	{
		fov = degree;
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
		return perspectiveModified || Camera::modified();
	}
private:
	GLfloat fov = 60.f, zNear = .1f, zFar = 100.f;
	GLint left = 0, top = 0, 
		width = WindowBase::getWidth(), height = WindowBase::getHeight();
	bool perspectiveModified = true;
	::glm::mat4 perspectiveTrans, trans;
	static ViewPort *activePort;
};

}