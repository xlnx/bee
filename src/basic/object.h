#pragma once

#include "gl.h"
#include "camera.h"
#include "basicControl.h"

namespace bee
{

class Object: public BasicCtrl
{
public:
	Object() = default;
	Object(const gl::Model &m, gl::Shader &s): 
		model(&m), shader(&s) 
	{}

	virtual void render()
	{
		shader->use();
		shader->uniform<::glm::mat4>("gWVP") = camera.getTrans() * getTrans();
		model->render();
	}
protected:
	const gl::Model *model = nullptr;
	gl::Shader *shader = nullptr;
};

}