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
protected:
	gl::Shader *shader = &gl::Shader::load(
		"model",
		gl::VertexShader("model-vs.glsl"),
		gl::FragmentShader("phong-fs.glsl")
	);
public:
	BEE_PROPERTY_REF(const gl::Model, Model) = nullptr;
};

class SelectiveModelObject: public ModelObject
{
	BEE_UNIFORM_GLOB(::glm::vec3, Color);
public:
	SelectiveModelObject() = default;
	SelectiveModelObject(const ::std::string &model):
		ModelObject(model)
	{
	}
	SelectiveModelObject(const gl::Model &model):
		ModelObject(model)
	{
	}

	void render(ViewPort &viewPort) override
	{
		if (isSelected)
		{
			shader->use();
				setViewMatrices(viewPort);
				glStencilFunc(GL_ALWAYS, 0, 0xff);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				fModel->render();
			shader->unuse();
			ringShader->use();
				setViewMatrices(viewPort);
				gColor = fSelectColor;
				glStencilFunc(GL_EQUAL, 0, 0xff);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
				fModel->render();
			ringShader->unuse();
			shader->use();
				setViewMatrices(viewPort);
				glStencilFunc(GL_NEVER, 0, 0xff);
				glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
				fModel->render();
			shader->unuse();
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}
		else if (isHovered)
		{
			shader->use();
				setViewMatrices(viewPort);
				glStencilFunc(GL_ALWAYS, 0, 0xff);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				fModel->render();
			shader->unuse();
			ringShader->use();
				setViewMatrices(viewPort);
				gColor = fHoverColor;
				glStencilFunc(GL_EQUAL, 0, 0xff);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
				fModel->render();
			ringShader->unuse();
			shader->use();
				setViewMatrices(viewPort);
				glStencilFunc(GL_NEVER, 0, 0xff);
				glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
				fModel->render();
			shader->unuse();
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}
		else
		{
			ModelObject::render(viewPort);
		}
	}
	void select(bool value = true)
	{
		isSelected = value;
	}
	bool selected() const
	{
		return isSelected;
	}
	void hover(bool value = true)
	{
		isHovered = value;
	}
	bool hovered() const
	{
		return isHovered;
	}
protected:
	gl::Shader *ringShader = &gl::Shader::load(
		"ring",
		gl::VertexShader("ring-vs.glsl"),
		gl::FragmentShader("ring-fs.glsl")
	);
protected:
	bool isSelected = false;
	bool isHovered = false;
public:
	BEE_PROPERTY(::glm::vec3, HoverColor) = ::glm::vec3(0, 0, 1);
	BEE_PROPERTY(::glm::vec3, SelectColor) = ::glm::vec3(1, 0, 0);
};

}