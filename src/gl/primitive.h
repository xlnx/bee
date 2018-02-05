#pragma once

#include "common.h"
#include "vertexAttr.h"

namespace bee
{

namespace gl
{

using Indices = ArrayMemoryManager<::glm::uvec3>;

class IBOPrimitive
{
public:
	IBOPrimitive(): IBO(0), VBO(0) {}
	template <typename ...Attrs>
	IBOPrimitive(const VertexAttrs<Attrs...> &vertices, const Indices &indices):
		posvecsize(sizeof(vertices[0].template get<position>()))
	{
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	}

	void draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, posvecsize, GL_UNSIGNED_INT, 0);
	}
private:
	GLuint IBO, VBO;
	::std::size_t posvecsize;
};

class Primitive
{
public:
	Primitive(): VBO(0) {}
	template <typename ...Attrs>
	Primitive(const VertexAttrs<Attrs...> &vertices):
		vertnum(vertices.size())
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	}

	void draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, vertnum);//3*sizeof(float), GL_UNSIGNED_INT, 0);
	}
private:
	GLuint VBO;
	int vertnum;
};

}

}