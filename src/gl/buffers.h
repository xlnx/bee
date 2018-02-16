#pragma once 

#include "common.h"
#include "vertexAttr.h"

namespace bee
{

namespace gl
{

using Faces = ArrayMemoryManager<::glm::uvec3>;

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
	void bind() noexcept
	{ glBindBuffer(target, handle); }
	void data(GLsizeiptr size, const void *ptr) noexcept
	{ glBufferData(target, size, ptr, GL_STATIC_DRAW); }
private:
	GLuint handle;
};

class VAO
{
public:
	operator GLuint() const noexcept
	{
		return handle;
	}
protected:
	GLuint handle = VertexArrayGenerator::gen();
};

class IndicedVAO: public VAO
{
public:
	template <typename ...Attrs>
	IndicedVAO(const VertexAttrs<Attrs...> &vertices, const Faces &faces):
		indicesCount(faces.size() * 3), info(vertices.info)
	{
		VBO vbo; EBO ebo;
		glBindVertexArray(handle);
			vbo.bind(); vbo.data(vertices.size() * vertices.elemSize, vertices.begin());
			ebo.bind(); ebo.data(faces.size() * faces.elemSize, faces.begin());
			vertices.performSetVertexAttribute();
		glBindVertexArray(0);
	}

	void render() const noexcept
	{
		glBindVertexArray(handle);
			info.invoke(); glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
private:
	GLsizei indicesCount;
	VertexAttrEnabledInfo info;
};

class ArrayedVAO: public VAO
{
public:
	void render(GLenum mode, int first, int count) const noexcept
	{
		glBindVertexArray(handle);
			info.invoke(); glDrawArrays(mode, first, count);
		glBindVertexArray(0);
	}
	template <typename ...Attrs>
	void setVertices(const VertexAttrs<Attrs...> &vertices)
	{
		info = vertices.info;
		glBindVertexArray(handle);
			vbo.bind(); vbo.data(vertices.size() * vertices.elemSize, vertices.begin());
			vertices.performSetVertexAttribute();
		glBindVertexArray(0);
	}
private:
	VBO vbo;
	VertexAttrEnabledInfo info;
};

}

}