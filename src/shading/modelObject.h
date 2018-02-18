#pragma once

#include "model.h"
#include "object.h"

namespace bee
{

class ModelObject: public Object
{
public:
	ModelObject()
	{
		createShader();
	}
	ModelObject(const gl::Model &model): 
		fModel(&model)
	{
		createShader();
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		fModel->render();
	}
private:
	void createShader()
	{
		if (!shader)
		{
			shader = new gl::Shader(
				gl::VertexShader("model-vs.glsl"),
				gl::FragmentShader("model-fs.glsl")
			);
		}
	}
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
	static gl::Shader *shader;
};

}