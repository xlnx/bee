#pragma once

#include "model.h"
#include "object.h"

namespace bee
{

class TestMeshObject: public Object
{
public:
	TestMeshObject() = default;
	TestMeshObject(const gl::Mesh &mesh): 
		fMesh(mesh)
	{
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		fMesh.render();
		shader->unuse();
	}
private:
	gl::Shader *shader = &gl::Shader::load(
		"testMesh",
		gl::VertexShader("testMesh-vs.glsl"),
		gl::FragmentShader("testMesh-fs.glsl")
	);
public:
	BEE_PROPERTY(gl::Mesh, Mesh);
};

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
		gl::FragmentShader("phong-fs.glsl")
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
		gl::FragmentShader("phong-fs.glsl")
	);
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
};

}