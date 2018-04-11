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

	virtual void renderSelected(ViewPort &viewPort)
	{
		ObjectType::render(viewPort);
	}
	virtual void renderHovered(ViewPort &viewPort)
	{
		ObjectType::render(viewPort);
	}
	virtual void renderNormal(ViewPort &viewPort)
	{
		ObjectType::render(viewPort);
	}
	void render(ViewPort &viewPort) final override
	{
		if (selected())
		{
			renderSelected(viewPort);
		}
		else if(hovered())
		{
			renderHovered(viewPort);
		}
		else
		{
			renderNormal(viewPort);
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

	void render(ViewPort &viewPort) override
	{
		if (enableRing)
		{
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			ObjectType::render(viewPort);
			glStencilFunc(GL_EQUAL, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			// gl::Shader::bind(*ringShader);
			ringShader->use();
			gColor = fRingColor;
			ObjectType::render(viewPort);
			// gl::Shader::unbind();
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
			ObjectType::render(viewPort);
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}
		else
		{
			ObjectType::render(viewPort);
		}
	}
	void ring(bool value)
	{
		enableRing = value;
	}
	void filpRing()
	{
		enableRing = !enableRing;
	}
protected:
	gl::Shader *ringShader = &gl::Shader::load(
		"ring",
		gl::VertexShader("ring-vs.glsl"),
		gl::FragmentShader("ring-fs.glsl")
	);
public:
	BEE_PROPERTY(::glm::vec3, RingColor) = ::glm::vec3(1, 0, 0);
	bool enableRing = false;
};

}