#pragma once

#include "object.h"
#include "mesh.h"
#include "texture.h"

namespace bee
{

class TestTexture2DObject: public Object
{
	BEE_UNIFORM_GLOB(int, Sampler);
public:
	TestTexture2DObject() = default;
	TestTexture2DObject(const gl::Texture2D &texture):
		fTexture(&texture)
	{
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		fTexture->invoke(1);
		gSampler = 1;
		mesh.render();
		shader->use();
	}
private:
	gl::Shader *shader = &gl::Shader::load(
		"testTexture2D",
		gl::VertexShader("testTexture2D-vs.glsl"),
		gl::FragmentShader("testTexture2D-fs.glsl")
	);
	gl::Mesh mesh = gl::Mesh(
		gl::VertexAttrs<gl::pos2>{
			::std::tuple<::glm::vec2>{{ -1, 1 }},
			::std::tuple<::glm::vec2>{{ 1, 1 }},
			::std::tuple<::glm::vec2>{{ 1, -1 }},
			::std::tuple<::glm::vec2>{{ -1, -1 }}
		},
		gl::Faces{
			{ 0, 1, 2 },
			{ 2, 3, 0 }
		}
	);
public:
	BEE_PROPERTY_REF(const gl::Texture2D, Texture) = nullptr;
};

class TestTexture3DObject: public Object
{
	BEE_UNIFORM_GLOB(int, Sampler);
	BEE_UNIFORM_GLOB(int, Target);
public:
	TestTexture3DObject() = default;
	TestTexture3DObject(const gl::Texture3D &texture):
		fTexture(&texture)
	{
	}

	void setTarget(GLuint target)
	{
		if (target > GL_TEXTURE_CUBE_MAP_POSITIVE_X)
		{
			target -= GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		}
		if (target > 5)
		{
			BEE_RAISE(GLFatal, "Invalid target");
		}
		this->target = target;
	}
	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		fTexture->invoke(1);
		gSampler = 1;
		gTarget = target;
		mesh.render();
		shader->unuse();
	}
private:
	gl::Shader *shader = &gl::Shader::load(
		"testTexture3D",
		gl::VertexShader("testTexture3D-vs.glsl"),
		gl::FragmentShader("testTexture3D-fs.glsl")
	);
	gl::Mesh mesh = gl::Mesh(
		gl::VertexAttrs<gl::pos2>{
			::std::tuple<::glm::vec2>{{ -1, 1 }},
			::std::tuple<::glm::vec2>{{ 1, 1 }},
			::std::tuple<::glm::vec2>{{ 1, -1 }},
			::std::tuple<::glm::vec2>{{ -1, -1 }}
		},
		gl::Faces{
			{ 0, 1, 2 },
			{ 2, 3, 0 }
		}
	);
	int target = 0;
public:
	BEE_PROPERTY_REF(const gl::Texture3D, Texture) = nullptr;
};

}
