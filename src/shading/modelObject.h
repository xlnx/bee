#pragma once

#include "model.h"
#include "object.h"

namespace bee
{

class ModelObject: public Object
{
public:
	ModelObject() = default;
	ModelObject(const gl::Model &model): 
		Object(!shader ? 
			*(shader = new gl::Shader(
				gl::VertexShader("model-vs.glsl"),
				gl::FragmentShader("model-fs.glsl"))
			) : *shader
		), fModel(&model)
	{
	}

	void render(ViewPort &viewPort) override
	{
		Object::render(viewPort);
		fModel->render();
	}
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
	static gl::Shader *shader;
};

}