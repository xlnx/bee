#pragma once

#include "shader.h"
#include "model.h"
#include "viewPort.h"
#include "basicControl.h"

namespace bee
{

class Object: public BasicCtrl
{
public:
	Object() = default;
	Object(const gl::Model &m, gl::Shader &s): 
		model(&m), shader(&s) 
	{
	}

	virtual void render(ViewPort &viewPort)
	{
		shader->use();
		shader->uniform<::glm::mat4>("gWVP") = 
			::glm::transpose(viewPort.getTrans() * getTrans());
		model->render(*shader);
	}
protected:
	const gl::Model *model = nullptr;
	gl::Shader *shader = nullptr;
};

}