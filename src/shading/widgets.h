#pragma once

#include "object.h"
#include "sceneWidgets.h"

namespace bee
{

template <typename ObjectType>
class SelectiveObject: public ObjectType, public SelectUtil
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<ObjectType, Types...>::value
			>::type>
	SelectiveObject(Types &&...args):
		ObjectType(::std::forward<Types>(args)...)
	{
	}

	virtual void renderSelected(Viewport &viewport)
	{
		ObjectType::render(viewport);
	}
	virtual void renderHovered(Viewport &viewport)
	{
		ObjectType::render(viewport);
	}
	virtual void renderNormal(Viewport &viewport)
	{
		ObjectType::render(viewport);
	}
	void render(Viewport &viewport) final override
	{
		if (selected())
		{
			renderSelected(viewport);
		}
		else if(hovered())
		{
			renderHovered(viewport);
		}
		else
		{
			renderNormal(viewport);
		}
	}
};

template <typename ObjectType>
class RingObject: public ObjectType
{
	BEE_UNIFORM_GLOB(::glm::vec3, Color);
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<ObjectType, Types...>::value
			>::type>
	RingObject(Types &&...args):
		ObjectType(::std::forward<Types>(args)...)
	{
	}

	void render(Viewport &viewport) override
	{
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		ObjectType::render(viewport);
		glStencilFunc(GL_EQUAL, 0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// gl::Shader::bind(*ringShader);
		ringShader->use();
		gColor = fRingColor;
		ObjectType::render(viewport);
		// gl::Shader::unbind();
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
		ObjectType::render(viewport);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
protected:
	gl::Shader *ringShader = &gl::Shader::load(
		"ring",
		gl::VertexShader("ring-vs.glsl"),
		gl::FragmentShader("ring-fs.glsl")
	);
public:
	BEE_PROPERTY(::glm::vec3, RingColor) = ::glm::vec3(1, 0, 0);
};

}