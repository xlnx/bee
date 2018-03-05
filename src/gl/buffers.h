#pragma once 

#include "common.h"
#include "vertexAttr.h"
#include <sstream>

namespace bee
{

namespace gl
{

using Faces = ArrayMemoryManager<::glm::uvec3>;

class BufferBase
{
public:
	virtual ~BufferBase()
	{
	}

	operator GLuint() const
	{
		return handle;
	}
protected:
	GLuint handle;
};

class VBO: public BufferBase
{
public:
	VBO()
	{
		glGenBuffers(1, &handle);
	}
	//~VBO()
	//{
		//glDeleteBuffers(1, &handle);
	//}
	void bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, handle);
	}
	void unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void data(GLsizeiptr size, const void *ptr) const
	{
		glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STATIC_DRAW);
	}
};

class EBO: public BufferBase
{
public:
	EBO()
	{
		glGenBuffers(1, &handle);
	}
	//~EBO()
	//{
		//glDeleteBuffers(1, &handle);
	//}
	void bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	}
	void unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void data(GLsizeiptr size, const void *ptr) const
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, ptr, GL_STATIC_DRAW);
	}
};

class VAO: public BufferBase
{
public:
	VAO()
	{
		glGenVertexArrays(1, &handle);
	}
	//~VAO()
	//{
		//glDeleteVertexArrays(1, &handle);
	//}
	void bind() const
	{
		glBindVertexArray(handle);
	}
	void unbind() const
	{
		glBindVertexArray(0);
	}
};

class FBO: public BufferBase
{
public:
	FBO()
	{
		glGenFramebuffers(1, &handle);
	}
	~FBO()
	{
		glDeleteFramebuffers(1, &handle);
	}
	void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, handle);
	}
	void unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	template <GLuint AttachType>
	void addTexture(GLuint texture) const
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, AttachType, GL_TEXTURE_2D, texture, 0);
	}
	void validate() const
	{
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			::std::ostringstream os;
			os << "Framebuffer validation failed: " << status;
			BEE_RAISE(GLFatal, os.str());
		}
	}
};

}

}
