#pragma once

#include "buffers.h"
#include "texture.h"

namespace bee
{

namespace gl
{

class IndicedVertices
{
public:
	template <typename ...Attrs>
	IndicedVertices(const VertexAttrs<Attrs...> &vertices, const Faces &faces):
		indicesCount(faces.size() * 3), info(vertices.info)
	{
		vao.bind();
			vbo.bind(); vbo.data(vertices.size() * vertices.elemSize, vertices.begin());
			ebo.bind(); ebo.data(faces.size() * faces.elemSize, faces.begin());
			vertices.performSetVertexAttribute();
		vao.unbind();
			vbo.unbind();
			ebo.unbind();
	}

	void render() const noexcept
	{
		vao.bind();
			info.invoke(); glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
		vao.unbind();
	}
private:
	VAO vao; VBO vbo; EBO ebo;
	GLsizei indicesCount;
	VertexAttrEnabledInfo info;
};

class ArrayedVertices
{
public:
	template <typename ...Attrs>
	void setVertices(const VertexAttrs<Attrs...> &vertices)
	{
		info = vertices.info;
		vao.bind();
			vbo.bind(); vbo.data(vertices.size() * vertices.elemSize, vertices.begin());
			vertices.performSetVertexAttribute();
		vao.unbind();
			vbo.unbind();
	}

	void render(GLenum mode, int first, int count) const noexcept
	{
		vao.bind();
			info.invoke(); glDrawArrays(mode, first, count);
		vao.unbind();
	}
private:
	VAO vao; VBO vbo;
	VertexAttrEnabledInfo info;
};

class DepthFramebuffer
{
public:
	DepthFramebuffer()
	{
		fbo.bind();
			texture.attachTo(fbo);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			fbo.validate();
		fbo.unbind();
	}
	void bind() const
	{
		fbo.bind();
	}
	void unbind() const
	{
		fbo.unbind();
	}
	void invoke(int i) const
	{
		return texture.invoke(i);
	}
private:
	FBO fbo;
	DepthTexture texture;
};

}

}