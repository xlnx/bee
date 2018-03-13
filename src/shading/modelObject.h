#pragma once

#include "model.h"
#include "object.h"

namespace bee
{

class MeshObject: public Object
{
public:
	MeshObject() = default;
	MeshObject(const gl::Mesh &mesh): 
		fMesh(&mesh)
	{
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		fMesh->render();
		shader->unuse();
	}
private:
	gl::Shader *shader = &gl::Shader::load(
		"model",
		gl::VertexShader("model-vs.glsl"),
		gl::FragmentShader("model-fs.glsl")
	);
public:
	BEE_PROPERTY_REF(const gl::Mesh, Mesh) = nullptr;
};

class ModelObject: public Object
{
public:
	ModelObject() = default;
	ModelObject(const ::std::string &model): 
		fModel(&gl::Model::load(model))
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
		shader->unuse();
	}
private:
	gl::Shader *shader = &gl::Shader::load(
		"model",
		gl::VertexShader("model-vs.glsl"),
		gl::FragmentShader("model-fs.glsl")
	);
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
};

}