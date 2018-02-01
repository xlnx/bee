#pragma once 

#include "common.h"
#include <vector>
#include <initializer_list>

namespace bee
{

namespace gl
{

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
			BEE_RAISE(GLFatal, "Failed to create shader program.");
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

}

}