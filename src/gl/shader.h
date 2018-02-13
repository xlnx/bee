#pragma once 

#include "common.h"
#include <map>

namespace bee
{

namespace gl
{

template <int ShaderType> class ShaderObj;
class Shader;

template <typename T> class UniformRef;



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

template <typename T>
class UniformRefBase
{
protected:
	friend class Shader;
	UniformRefBase() = default;
	UniformRefBase &operator = (const UniformRefBase &other) = delete;
public:
	void bind(const UniformRefBase &other)
	{
		handle = other.handle;
	}
	operator bool() const
	{
		return ~handle;
	}
protected:
	GLuint handle = -1;
};

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
		UniformRef<T> uniform(const std::string &name)
	{
		UniformRef<T> ref;
		if (!bufferedUniforms.count(name))
		{
			auto handle = glGetUniformLocation(shader, name.c_str());
			if (!~handle)
			{
				BEE_RAISE(GLFatal, "Invalid uniform variable name: " + name);
			}
			bufferedUniforms[name] = handle;
		}
		static_cast<UniformRefBase<T>&>(ref).handle = bufferedUniforms[name];
		return ref;
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
		glValidateProgram(shader);
		glGetProgramiv(shader, GL_VALIDATE_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::ostringstream os;
			os << "Invalid shader program." << std::endl << infoLog;
			BEE_RAISE(GLFatal, os.str());
		}
	}
private:
	GLuint shader;
	::std::map<::std::string, GLuint> bufferedUniforms;
};

template <typename T>
	struct UniformRef: UniformRefBase<T>
{
	void operator = (const T &target);
};

template <>
	struct UniformRef<::glm::mat2>: UniformRefBase<::glm::mat2>
{
	void operator = (const ::glm::mat2 &target)
	{
		glUniformMatrix2fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRef<::glm::mat3>: UniformRefBase<::glm::mat3>
{
	void operator = (const ::glm::mat3 &target)
	{
		glUniformMatrix3fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRef<::glm::mat4>: UniformRefBase<::glm::mat4>
{
	void operator = (const ::glm::mat4 &target)
	{
		glUniformMatrix4fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRef<int>: UniformRefBase<int>
{
	void operator = (const int &target)
	{
		glUniform1i(handle, target);
	}
};

template <>
	struct UniformRef<float>: UniformRefBase<float>
{
	void operator = (const float &target)
	{
		glUniform1f(handle, target);
	}
};

template <>
	struct UniformRef<::glm::vec2>: UniformRefBase<::glm::vec2>
{
	void operator = (const ::glm::vec2 &target)
	{
		glUniform2f(handle, target[0], target[1]);
	}
};

template <>
	struct UniformRef<::glm::vec3>: UniformRefBase<::glm::vec3>
{
	void operator = (const ::glm::vec3 &target)
	{
		glUniform3f(handle, target[0], target[1], target[2]);
	}
};

template <>
	struct UniformRef<::glm::vec4>: UniformRefBase<::glm::vec4>
{
	void operator = (const ::glm::vec4 &target)
	{
		glUniform4f(handle, target[0], target[1], target[2], target[3]);
	}
};

}

}