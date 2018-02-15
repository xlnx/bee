#pragma once

#include "shader.h"
#include "model.h"
#include "viewPort.h"
#include "objectBase.h"
#include "property.h"

namespace bee
{

class Object: public ObjectBase
{
	BEE_UNIFORM_GLOB(::glm::mat4, WVP);
	BEE_UNIFORM_GLOB(::glm::mat4, World);
	BEE_UNIFORM_GLOB(::glm::vec3, CameraWorldPos);
protected:
	Object() = default;
	Object(gl::Shader &shader):
		fShader(&shader)
	{
	}
public:
	virtual void render(ViewPort &viewPort)
	{
		fShader->use();
		gWVP = ::glm::transpose(viewPort.getTrans() * getTrans());
		gWorld = ::glm::transpose(getTrans());
		gCameraWorldPos = viewPort.getPosition();
	}
public:
	BEE_PROPERTY_REF(gl::Shader, Shader) = nullptr;
};

class ModelObject: public Object
{
public:
	ModelObject() = default;
	ModelObject(const gl::Model &model, gl::Shader &shader): 
		Object(shader), fModel(&model)
	{
	}

	void render(ViewPort &viewPort) override
	{
		Object::render(viewPort);
		fModel->render();
	}
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
};

}