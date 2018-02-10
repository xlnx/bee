#pragma once

#include <glm/glm.hpp>
#include <utility>

namespace bee
{

class Camera
{
protected:
	Camera(::glm::vec3 target = {0, 1, 0},
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
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setUp(Types &&...args)
	{
		Up = ::glm::normalize(::glm::vec3(::std::forward<Types>(args)...));
		rotateModified = true;
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

}