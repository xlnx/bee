#pragma once

#include "shader.h"
#include "viewPort.h"
#include "property.h"
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include <functional>

namespace bee
{

class ObjectBase
{
protected:
	ObjectBase() = default;
	virtual ~ObjectBase() = default;
public:
	const ::glm::mat4 &getTrans()
	{
		if (translateModified || rotateModified || scaleModified)
		{
			if (translateModified)
			{
				translateModified = false;
				translateTrans = {
					1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					P.x, P.y, P.z, 1.f
				};
			}
			if (rotateModified)
			{
				rotateModified = false;
				auto sinx = sinf(R.x), siny = sinf(R.y), sinz = sinf(R.z),
					cosx = cosf(R.x), cosy = cosf(R.y), cosz = cosf(R.z),
					sycz = siny * cosz, sysz = siny * sinz;
				rotateTrans = {
					cosz * cosy         , sinz * cosy         , siny     , 0.f,
					-cosx*sinz-sinx*sycz, cosx*cosz-sinx*sysz , sinx*cosy, 0.f,
					sinx*sinz-cosx*sycz , -sinx*cosz-cosx*sysz, cosx*cosy, 0.f,
					0.f                 , 0.f                 , 0.f      , 1.f
				};
			}
			if (scaleModified)
			{
				scaleModified = false;
				scaleTrans = {
					S.x, 0.f, 0.f, 0.f,
					0.f, S.y, 0.f, 0.f,
					0.f, 0.f, S.z, 0.f,
					0.f, 0.f, 0.f, 1.f
				};
			}
			trans = translateTrans * rotateTrans * scaleTrans;
		}
		return trans;
	}
public:
	// scale
	void scaleIdentity()
	{
		S = {1, 1, 1};
		scaleModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void scale(Types &&...args)
	{
		S *= ::glm::vec3(::std::forward<Types>(args)...);
		scaleModified = true;
	}
	// translate
	void translateIdentity()
	{
		P = {0, 0, 0};
		translateModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void translate(Types &&...args)
	{
		P += ::glm::vec3(::std::forward<Types>(args)...);
		translateModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setPosition(Types &&...args)
	{
		P = ::glm::vec3(::std::forward<Types>(args)...);
		translateModified = true;
	}
	// rotate
	void rotateIdentity()
	{
		R = {0, 0, 0};
		rotateModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void rotate(Types &&...args)
	{
		R += ::glm::vec3(::std::forward<Types>(args)...);
		rotateModified = true;
	}
protected:
	bool translateModified = true, rotateModified = true, scaleModified = true;
	::glm::vec3 P = {0, 0, 0}, S = {1, 1, 1}, R = {0, 0, 0};
private:
	::glm::mat4 translateTrans, rotateTrans, scaleTrans, trans;
};

class Transform: public ObjectBase
{
};

class Object: public ObjectBase
{
	BEE_UNIFORM_GLOB(::glm::mat4, WVP);
	BEE_UNIFORM_GLOB(::glm::mat4, World);
	BEE_UNIFORM_GLOB(::glm::mat4, VP);
	BEE_UNIFORM_GLOB(::glm::vec3, CameraWorldPos);
	BEE_UNIFORM_GLOB(float, Time);
protected:
	Object() = default;
	void setViewMatrices(ViewPort &viewPort)
	{
		gTime = float(glfwGetTime());
		gWVP = ::glm::transpose(viewPort.getTrans() * getTrans());
		gWorld = ::glm::transpose(getTrans());
		gVP = ::glm::transpose(viewPort.getTrans());
		gCameraWorldPos = viewPort.getPosition();
		for (auto &f: getSetCallbacks())
		{
			f(*this, viewPort);
		}
	}
private:
	static ::std::vector<::std::function<void(Object&, ViewPort&)>> &
		getSetCallbacks()
	{
		static ::std::vector<::std::function<void(Object&, ViewPort&)>> r;
		return r;
	}
public:
	virtual void render(ViewPort &viewPort) = 0;
public:
	static void onSetViewMatrices(const ::std::function<void(Object&, ViewPort&)> &f)
	{
		getSetCallbacks().emplace_back(f);
	}
};

}