#pragma once

#include "model.h"
#include "object.h"
#include "modelManager.h"

namespace bee
{

class ModelObject: public Object
{
public:
	ModelObject() = default;
	ModelObject(const ::std::string &model): 
		fModel(&ModelManager::loadModel(model))
	{
	}
	ModelObject(const gl::Model &model): 
		fModel(&model)
	{
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		fModel->render();
	}
private:
	static gl::Shader *getShader()
	{
		static auto var = new gl::Shader(
			gl::VertexShader("model-vs.glsl"),
			gl::FragmentShader("model-fs.glsl")
		);
		return var;
	}
private:
	gl::Shader *shader = getShader();
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
};

}