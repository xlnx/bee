#pragma once

#include "shader.h"

namespace bee
{

class ShaderController
{
protected:
	ShaderController() = default;
public:
	virtual void invoke() = 0;
};

}