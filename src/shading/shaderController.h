#pragma once

#include "shader.h"
#include <vector>
#include <map>
#include <functional>

namespace bee
{

struct ShaderControllerInfo
{
	gl::UniformRef<int> counter;
	const char *typeName;
};
using ShaderControllerInfoGetter = ShaderControllerInfo *(*)();

class ShaderController
{
protected:
	ShaderController() = default;
public:
	virtual ~ShaderController() = default;
	
	virtual ShaderControllerInfoGetter getInfoFunc() = 0;
public:
	virtual bool invoke(int index) = 0;
};

class ShaderControllerSingle: public ShaderController
{
protected:
	static constexpr bool isMulti = false;
protected:
	ShaderControllerSingle(const ::std::string &prefix): 
		prefix(prefix)
	{
	}
private:
	const ::std::string &prefix = "ShaderControllerSingle";
};

class ShaderControllerMulti: public ShaderController
{
protected:
	static constexpr bool isMulti = true;
protected:
	ShaderControllerMulti(const ::std::string &prefix): 
		prefix(prefix)
	{
	}
private:
	const ::std::string &prefix = "ShaderControllerMulti[]";
};

template <typename T = ShaderController, typename = typename 
	::std::enable_if<::std::is_base_of<ShaderController, T>::value>::type>
class ShaderControllers final
{
public:
	void invoke()
	{
		::std::map<ShaderControllerInfo*, int> ptrs;
		for (auto & controller: controllers)
		{
			if (auto multi = dynamic_cast<ShaderControllerMulti*>(controller))
			{
				auto info = multi->getInfoFunc()();
				if (multi->invoke(ptrs[info]))
				{
					++ptrs[info];
				}
			}
			else
			{
				controller->invoke(0);
			}
		}
		for (auto &pair: ptrs)
		{
			pair.first->counter = pair.second;
		}
	}
	void addController(T &controller)
	{
		controllers.push_back(&controller);
	}
	void foreach(const ::std::function<void(T*)> &f)
	{
		for (auto controller: controllers)
		{
			f(controller);
		}
	}
protected:
	::std::vector<T *> controllers;
};

}