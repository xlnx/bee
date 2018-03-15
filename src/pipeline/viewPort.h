#pragma once

#include "common.h"
#include "windowBase.h"
#include "camera.h"

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
		if (activePort() != this)
		{
			activePort() = this;
#			ifdef __APPLE__
			int w, h;
			glfwGetFramebufferSize(WindowBase::getInstance(), &w, &h);
			glViewport(left, top, w, h);
#			endif
		}
		if (modified())
		{
			if (perspectiveModified)
			{
				perspectiveModified = false;
				perspectiveTrans = ::glm::perspective(fov, float(width) / height, zNear, zFar);
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
		return perspectiveModified || Camera::modified();
	}
	static ViewPort *&activePort()
	{
		static ViewPort *port = nullptr;
		return port;
	}
private:
	GLfloat fov = ::glm::radians(60.f), zNear = 1e-3f, zFar = 1e5f;//100.f;
	GLint left = 0, top = 0, 
		width = WindowBase::getWidth(), height = WindowBase::getHeight();
	bool perspectiveModified = true;
	::glm::mat4 perspectiveTrans, trans;
};

}