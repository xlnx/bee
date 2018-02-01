#pragma once 

#include "common.h"

namespace bee
{

namespace gl
{

template <int ShaderType> class ShaderObj;
class Shader;

template <typename T> class Ref;



template <int ShaderType>
	class ShaderObj final
{
public:
	ShaderObj(const std::string &filename):
		shader(glCreateShader(ShaderType))
	{
		if (!shader)
		{
			BEE_RAISE(GLFatal, "Failed to create shader.");
		}
		std::ifstream fin(filename);
		if (!fin)
		{
			BEE_RAISE(Fatal, "Shader file not found: " + filename);
		}
		std::string line, src;
		while (std::getline(fin, line))
		{
			src += line + '\n';
		}
		auto psrc = src.c_str();
		GLint success;
		GLchar infoLog[512];
		glShaderSource(shader, 1, &psrc, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::ostringstream os;
			os << "Shader compilation failed: " << filename << std::endl << infoLog;
			BEE_RAISE(GLFatal, os.str());
		}
	}
	ShaderObj(const ShaderObj &) = delete;
	ShaderObj(ShaderObj &&other):
		shader(other.shader)
	{
		other.shader = 0;
	}
	// ShaderObj(ShaderObj &&):
	// 	shader(other.shader)
	// {
	// 	other.shader = 0;
	// }
	~ShaderObj()
	{
		glDeleteShader(shader);
	}
	operator GLuint() const
	{
		return shader;
	}
private:
	GLuint shader;
};

using VertexShader = ShaderObj<GL_VERTEX_SHADER>;
using FragmentShader = ShaderObj<GL_FRAGMENT_SHADER>;
using GeometryShader = ShaderObj<GL_GEOMETRY_SHADER>;

class Shader final
{
public:
	template <typename ...Types>
	Shader(Types &&...args):
		shader(glCreateProgram())
	{
		if (!shader)
		{
			BEE_RAISE(GLFatal, "Failed to create shader program.");
		}
		attach(std::forward<Types>(args)...);
		link();
	}
	template <template <typename T, typename _Alloc> class E,
		typename Alloc>
	Shader(const E<GLuint, Alloc> &v)
	{
		for (auto e: v) attach(e);
		link();
	}
	Shader(const Shader &) = delete;
	Shader(Shader &&other):
		shader(other.shader)
	{
		other.shader = 0;
	}
	~Shader()
	{
		glDeleteProgram(shader);
	}
	
	operator GLuint() const
	{
		return shader;
	}
	void use() const
	{
		glUseProgram(shader);
	}
	template <typename T>
		Ref<T> var(const std::string &name)
	{
		auto handle = glGetUniformLocation(shader, name.c_str());
		if (!~handle)
		{
			BEE_RAISE(GLFatal, "Invalid uniform variable name: " + name);
		}
		return Ref<T>(handle);
	}
	template <typename T>
		Ref<T> var(const char *name)
	{
		auto handle = glGetUniformLocation(shader, name);
		if (!~handle)
		{
			BEE_RAISE(GLFatal, std::string("Invalid uniform variable name: ") + name);
		}
		return Ref<T>(handle);
	}
private:
	template <typename T, typename ...Types, typename = typename
		std::enable_if<std::is_constructible<GLuint, T>::value>::type>
	void attach(T shaderObj, Types &&...others)
	{
		glAttachShader(shader, shaderObj);
		attach(std::forward<Types>(others)...);
	}
	template <typename T, typename = typename
		std::enable_if<std::is_constructible<GLuint, T>::value>::type>
	void attach(T shaderObj)
	{
		glAttachShader(shader, shaderObj);
	}
	void link() const
	{
		GLint success;
		GLchar infoLog[512];
		glLinkProgram(shader);
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::ostringstream os;
			os << "Shader link failed." << std::endl << infoLog;
			BEE_RAISE(GLFatal, os.str());
		}
	}
private:
	GLuint shader;
};

template <typename T>
	class Ref
{
	friend class Shader;
public:
	Ref(): handle(-1) {}
	
	Ref &operator = (const Ref &other) = delete;
	void operator = (const T &target);
	void bind(const Ref &other)
	{
		handle = other.handle;
	}
private:
	Ref(GLuint handle): handle(handle) {}
private:
	GLuint handle;
};

template <>
	void Ref<::glm::mat2>::operator = (const ::glm::mat2 &target)
{
	glUniformMatrix2fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
}

template <>
	void Ref<::glm::mat3>::operator = (const ::glm::mat3 &target)
{
	glUniformMatrix3fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
}

template <>
	void Ref<::glm::mat4>::operator = (const ::glm::mat4 &target)
{
	glUniformMatrix4fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
}

template <>
	void Ref<float>::operator = (const float &target)
{
	glUniform1f(handle, target);
}

template <>
	void Ref<::glm::vec2>::operator = (const ::glm::vec2 &target)
{
	glUniform2f(handle, target[0], target[1]);
}

template <>
	void Ref<::glm::vec3>::operator = (const ::glm::vec3 &target)
{
	glUniform3f(handle, target[0], target[1], target[2]);
}

template <>
	void Ref<::glm::vec4>::operator = (const ::glm::vec4 &target)
{
	glUniform4f(handle, target[0], target[1], target[2], target[3]);
}

}

}