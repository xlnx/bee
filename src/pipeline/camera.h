#pragma once

#include <glm/glm.hpp>

namespace bee
{

class Camera
{
public:
	Camera():
		N{0, 0, -1}, V{0, 1, 0}, P{0, 0, 1}
	{}

	const ::glm::mat4 &getTrans()
	{
		if (modified())
		{
			trans = getRotateTrans() * getTranslationTrans();
		}
		return trans;
	}
	const ::glm::mat4 &getRotateTrans()
	{
		if (rotateModified)
		{
			rotateModified = false;
			auto U = ::glm::cross(N, V);
			rotateTrans = { 
				N.x, N.y, N.z, 0.f, 
				V.x, V.y, V.z, 0.f, 
				U.x, U.y, U.z, 0.f, 
				0.f, 0.f, 0.f, 1.f
			};
		}
		return rotateTrans;
	}
	const ::glm::mat4 &getTranslationTrans()
	{
		if (translationModified)
		{
			translationModified = false;
			translationTrans = {
				1.f, 0.f, 0.f, P.x,
				0.f, 1.f, 0.f, P.y,
				0.f, 0.f, 1.f, P.z,
				0.f, 0.f, 0.f, 1.f
			};
		}
		return translationTrans;
	}
	void setTarget(const ::glm::vec3 &target)
	{
		N = ::glm::normalize(target);
		rotateModified = true;
	}
	void setUp(const ::glm::vec3 &up)
	{
		V = ::glm::normalize(up);
		rotateModified = true;
	}
	void setPosition(const ::glm::vec3 &position)
	{
		P = position;
		translationModified = true;
	}
	bool modified() const
	{
		return rotateModified || translationModified;
	}
protected:
	bool rotateModified = true, translationModified = true;
	::glm::vec3 N, V, P;
	::glm::mat4 rotateTrans, translationTrans, trans;
}; 

class Pipeline
{
	Pipeline() = default;
public:
	const ::glm::mat4 &getTrans()
	{
		if (perspective.modified() || camera.modified())
		{
			trans = persProjTrans * camera.getTrans() * object.trans();
		}
		return trans;
	}
private:
	::glm::mat4 trans;
	Camera camera;
};

}