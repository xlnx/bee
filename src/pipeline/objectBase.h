#pragma once

#include <glm/glm.hpp>
#include <utility>

namespace bee
{

class ObjectBase
{
protected:
	ObjectBase() = default;
	virtual ~ObjectBase() = default;

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

}