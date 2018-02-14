#pragma once

#include "shader.h"
#include "model.h"
#include "viewPort.h"
#include "basicControl.h"
#include "property.h"

namespace bee
{

class Object: public BasicCtrl
{
	BEE_UNIFORM_GLOB(::glm::mat4, WVP);
	BEE_UNIFORM_GLOB(::glm::mat4, World);
	BEE_UNIFORM_GLOB(::glm::vec3, CameraWorldPos);
public:
	Object() = default;
	Object(const gl::Model &m, gl::Shader &s): 
		model(&m), shader(&s)
	{
	}

	virtual void render(ViewPort &viewPort)
	{
		shader->use();
		gWVP = ::glm::transpose(viewPort.getTrans() * getTrans());
		gWorld = ::glm::transpose(getTrans());
		gCameraWorldPos = viewPort.getPosition();
		model->render();
	}
protected:
	const gl::Model *model = nullptr;
	gl::Shader *shader = nullptr;
};

}