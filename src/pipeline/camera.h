#pragma once

#include <glm/glm.hpp>
#include <utility>

namespace bee
{

class Camera
{
public:
	Camera() = default;

	const ::glm::mat4 &getTrans()
	{
		if (modified())
		{
			trans = getRotateTrans() * getTranslationTrans();
		}
		return trans;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setTarget(Types &&...args)
	{
		N = ::glm::normalize(::glm::vec3(::std::forward<Types>(args)...));
		rotateModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setUp(Types &&...args)
	{
		V = ::glm::normalize(::glm::vec3(::std::forward<Types>(args)...));
		rotateModified = true;
	}
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setPosition(Types &&...args)
	{
		P = ::glm::vec3(::std::forward<Types>(args)...);
		translateModified = true;
	}
protected:
	const ::glm::mat4 &getRotateTrans()
	{
		if (rotateModified)
		{
			rotateModified = false;
			auto U = ::glm::cross(N, V);
			rotateTrans = { 
				U.x, U.y, U.z, 0.f, 
				V.x, V.y, V.z, 0.f, 
				N.x, N.y, N.z, 0.f, 
				0.f, 0.f, 0.f, 1.f
			};
		}
		return rotateTrans;
	}
	const ::glm::mat4 &getTranslationTrans()
	{
		if (translateModified)
		{
			translateModified = false;
			translateTrans = {
				1.f, 0.f, 0.f, P.x,
				0.f, 1.f, 0.f, P.y,
				0.f, 0.f, 1.f, P.z,
				0.f, 0.f, 0.f, 1.f
			};
		}
		return translateTrans;
	}
	bool modified() const
	{
		return rotateModified || translateModified;
	}
protected:
	bool rotateModified = true, translateModified = true;
	::glm::vec3 N = {0, 0, -1}, V = {0, 1, 0}, P = {0, 0, 1};
private:
	::glm::mat4 rotateTrans, translateTrans, trans;
}; 

extern Camera camera;

}