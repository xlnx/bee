#pragma once 

#include "common.h"
#include "vertexAttr.h"

namespace bee
{

namespace gl
{

using Indices = ArrayMemoryManager<::glm::uvec3>;

template <GLenum target>
class BufferObject;

class VAO;

using VBO = BufferObject<GL_ARRAY_BUFFER>;

using EBO = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;

class BufferGenerator
{
	friend class BufferObject<GL_ARRAY_BUFFER>;
	friend class BufferObject<GL_ELEMENT_ARRAY_BUFFER>;
	constexpr static std::size_t initSize = 4;
public:
	BufferGenerator()
	{
		ptr = dataptr = new GLuint [initSize];
		dataend = dataptr + initSize;
		if (haveInstance)
		{
			BEE_RAISE(GLFatal, "Cannot create SINGLETON class bee::gl::BufferGenerator.");
		}
		else
		{
			haveInstance = true;
		}
		glGenBuffers(initSize, dataptr);
	}
	BufferGenerator(const BufferGenerator &) = delete;
	~BufferGenerator()
	{
		delete [] dataptr;
	}
private:
	static GLuint gen()
	{
		if (ptr != dataend)
		{
			// glBindBuffer(Ty, *ptr);
			return *ptr++;
		}
		else
		{
			::std::size_t size = (dataend - dataptr) << 1;
			delete [] dataptr;
			ptr = dataptr = new GLuint [size];
			dataend = dataptr + size;
			glGenBuffers(size, dataptr);
			// glBindBuffer(Ty, *ptr);
			return *ptr++;
		}
	}
private:
	static bool haveInstance;
	static GLuint *dataptr;
	static GLuint *dataend;
	static GLuint *ptr;
};

class VertexArrayGenerator
{
	friend class VAO;
	constexpr static std::size_t initSize = 4;
public:
	VertexArrayGenerator()
	{
		ptr = dataptr = new GLuint [initSize];
		dataend = dataptr + initSize;
		if (haveInstance)
		{
			BEE_RAISE(GLFatal, "Cannot create SINGLETON class bee::gl::VertexArrayGenerator.");
		}
		else
		{
			haveInstance = true;
		}
		glGenVertexArrays(initSize, dataptr);
	}
	VertexArrayGenerator(const VertexArrayGenerator &) = delete;
	~VertexArrayGenerator()
	{
		delete [] dataptr;
	}
private:
	static GLuint gen()
	{
		if (ptr != dataend)
		{
			return *ptr++;
		}
		else
		{
			::std::size_t size = (dataend - dataptr) << 1;
			delete [] dataptr;
			ptr = dataptr = new GLuint [size];
			dataend = dataptr + size;
			glGenVertexArrays(size, dataptr);
			return *ptr++;
		}
	}
private:
	static bool haveInstance;
	static GLuint *dataptr;
	static GLuint *dataend;
	static GLuint *ptr;
};

template <GLenum target>
class BufferObject
{
public:
	BufferObject(): handle(BufferGenerator::gen())
	{}
	operator GLuint () const noexcept
	{ return handle; }
	void bind() const noexcept
	{ glBindBuffer(target, handle); }
	void data(GLsizeiptr size, const void *ptr) const noexcept
	{ glBufferData(target, size, ptr, GL_STATIC_DRAW); }
private:
	GLuint handle;
};

class VAO
{
public:
	template <typename ...Attrs>
	VAO(const VertexAttrs<Attrs...> &vertices, const Indices &indices):
		handle(VertexArrayGenerator::gen()), indicesCount(indices.size() * 3), info(vertices.info)
	{
		VBO vbo; EBO ebo;
		glBindVertexArray(handle);
			vbo.bind(); vbo.data(vertices.size() * sizeof(vertices[0]), vertices.data());
			ebo.bind(); ebo.data(indices.size() * sizeof(indices[0]), indices.data());
			vertices.setVertexAttribute();
		glBindVertexArray(0);
	}
	operator GLuint() const noexcept
	{
		return handle;
	}
	void render() const noexcept
	{
		glBindVertexArray(handle);
			info.invoke(); glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
private:
	GLuint handle;
	GLsizei indicesCount;
	VertexAttrEnabledInfo info;
};

}

}