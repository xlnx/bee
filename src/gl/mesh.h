#pragma once

#include "common.h"
#include "buffers.h"
#include "vertexAttr.h"

namespace bee
{

namespace gl
{

class MeshBase
{
public:
	MeshBase() = default;
	virtual ~MeshBase() = default;

	virtual void render() const = 0;
};

class Mesh: public MeshBase
{
public:
	// IBOPrimitive(): ebo(0), vbo(0) {}
	template <typename ...Attrs>
	Mesh(const VertexAttrs<Attrs...> &vertices, const Indices &indices):
		vao(vertices, indices)
	{}

	void render() const override
	{
		vao.render();
	}
private:
	VAO vao;
};

// class MeshBase
// {
// public:
// 	MeshBase(): vbo(0) {}
// 	template <typename ...Attrs>
// 	MeshBase(const VertexAttrs<Attrs...> &vertices):
// 		vertnum(vertices.size()), vbo(Buffer::template gen<VBO>())
// 	{
// 		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
// 	}

// 	void render()
// 	{
// 		glBindBuffer(GL_ARRAY_BUFFER, vbo);
// 		glDrawArrays(GL_TRIANGLES, 0, vertnum);//3*sizeof(float), GL_UNSIGNED_INT, 0);
// 	}
// private:
// 	GLuint vbo;
// 	int vertnum;
// };

}

}