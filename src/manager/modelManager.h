#pragma once

#include "model.h"
#include <map>

namespace bee
{

class ModelManager
{
	ModelManager() = default;
public:
	static gl::Model &loadModel(const ::std::string &name)
	{
		static ::std::map<::std::string, gl::Model*> loaded;
		if (auto model = loaded[name])
		{
			return *model;
		}
		else
		{
			return *(loaded[name] = new gl::Model(name));
		}
	}
};

}