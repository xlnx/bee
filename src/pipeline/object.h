#pragma once

#include "shader.h"
#include "viewport.h"
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
	// template <typename ...Types, typename = typename
	// 	::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	virtual void translate(float dx, float dy, float dz) //Types &&...args)
	{
		P += ::glm::vec3(dx, dy, dz);
		translateModified = true;
	}
	virtual void translate(const ::glm::vec3 &diff) //Types &&...args)
	{
		P += diff;
		translateModified = true;
	}
	// template <typename ...Types, typename = typename
		// ::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	virtual void setPosition(float dx, float dy, float dz)
	{
		P = ::glm::vec3(dx, dy, dz);
		translateModified = true;
	}
	virtual void setPosition(const ::glm::vec3 &pos)
	{
		P = pos;
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
	void setViewMatrices(Viewport &viewport)
	{
		gTime = float(glfwGetTime());
		gWVP = ::glm::transpose(viewport.getTrans() * getTrans());
		gWorld = ::glm::transpose(getTrans());
		gVP = ::glm::transpose(viewport.getTrans());
		gCameraWorldPos = viewport.getPosition();
		for (auto &f: getSetCallbacks())
		{
			f(*this, viewport);
		}
	}
private:
	static ::std::vector<::std::function<void(Object&, Viewport&)>> &
		getSetCallbacks()
	{
		static ::std::vector<::std::function<void(Object&, Viewport&)>> r;
		return r;
	}
public:
	virtual void render(Viewport &viewport) = 0;
public:
	static void onSetViewMatrices(const ::std::function<void(Object&, Viewport&)> &f)
	{
		getSetCallbacks().emplace_back(f);
	}
};

}