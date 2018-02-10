#pragma once

#include <glm/glm.hpp>
#include <utility>

namespace bee
{

class BasicCtrl
{
protected:
	BasicCtrl() = default;
	virtual ~BasicCtrl() = default;

public:
	const ::glm::mat4 &getTrans()
	{
		if (modified())
		{
			trans = getTranslateTrans() * getRotateTrans() * getScaleTrans();
		}
		return trans;
	}
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
	const ::glm::mat4 &getRotateTrans()
	{
		if (rotateModified)
		{
			rotateModified = false;
			// rotateTrans = ::glm::mat4{
			// 	cosf(R.z), -sinf(R.z), 0.f, 0.f,
			// 	sinf(R.z), cosf(R.z), 0.f, 0.f,
			// 	0.f, 0.f, 1.f, 0.f,
			// 	0.f, 0.f, 0.f, 1.f
			// } * ::glm::mat4{
			// 	cosf(R.y), 0.f, -sinf(R.y), 0.f,
			// 	0.f, 1.f, 0.f, 0.f,
			// 	sinf(R.y), 0.f, cosf(R.y), 0.f,
			// 	0.f, 0.f, 0.f, 1.f
			// } * ::glm::mat4{
			// 	1.f, 0.f, 0.f, 0.f,
			// 	0.f, cosf(R.x), -sinf(R.x), 0.f,
			// 	0.f, sinf(R.x), cosf(R.x), 0.f,
			// 	0.f, 0.f, 0.f, 1.f
			// };
			auto sinx = sinf(R.x), siny = sinf(R.y), sinz = sinf(R.z),
				cosx = cosf(R.x), cosy = cosf(R.y), cosz = cosf(R.z),
				sycz = siny * cosz, sysz = siny * sinz;
			rotateTrans = {
				cosz * cosy, -cosx*sinz-sinx*sycz, sinx*sinz-cosx*sycz , 0.f,
				sinz * cosy, cosx*cosz-sinx*sysz , -sinx*cosz-cosx*sysz, 0.f,
				siny       , sinx*cosy           , cosx*cosy           , 0.f,
				0.f        , 0.f                 , 0.f                 , 1.f
			};
		}
		return rotateTrans;
	}
	const ::glm::mat4 &getTranslateTrans()
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
	const ::glm::mat4 &getScaleTrans()
	{
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
		return scaleTrans;
	}
	bool modified() const
	{
		return rotateModified || translateModified || scaleModified;
	}
protected:
	bool translateModified = true, rotateModified = true, scaleModified = true;
	::glm::vec3 P = {0, 0, 0}, S = {1, 1, 1}, R = {0, 0, 0};
private:
	::glm::mat4 translateTrans, rotateTrans, scaleTrans, trans;
};

}