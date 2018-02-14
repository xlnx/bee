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
		model(&m), shader(&s), 
		gWVP(gl::Shader::uniform<::glm::mat4>("gWVP")), 
		gWorld(gl::Shader::uniform<::glm::mat4>("gWorld"))
	{
	}

	virtual void render(ViewPort &viewPort)
	{
		shader->use();
		gWVP = ::glm::transpose(viewPort.getTrans() * getTrans());
		gWorld = ::glm::transpose(getTrans());
		model->render();
	}
protected:
	const gl::Model *model = nullptr;
	gl::Shader *shader = nullptr;
	gl::UniformRef<::glm::mat4> gWVP, gWorld;
};

}