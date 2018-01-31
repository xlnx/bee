#pragma once 

#include "common.h"

namespace bee
{

namespace gl
{

template <int ShaderType>
	class ShaderPiece final
{
public:
	ShaderPiece(const std::string &filename):
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
		glShaderSource(shader, 1, &psrc, nullptr);
		glCompileShader(shader);
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::ostringstream os;
			os << "Shader compilation failed: " << filename << std::endl << infoLog;
			BEE_RAISE(GLFatal, os.str());
		}
	}
	ShaderPiece(const ShaderPiece &) = delete;
	ShaderPiece(ShaderPiece &&other):
		shader(other.shader)
	{
		other.shader = 0;
	}
	// ShaderPiece(ShaderPiece &&):
	// 	shader(other.shader)
	// {
	// 	other.shader = 0;
	// }
	~ShaderPiece()
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

using VertexShader = ShaderPiece<GL_VERTEX_SHADER>;
using FragmentShader = ShaderPiece<GL_FRAGMENT_SHADER>;
using GeometryShader = ShaderPiece<GL_GEOMETRY_SHADER>;

class Shader final
{
public:
	Shader(): 
		shader(glCreateProgram())
	{
		if (!shader)
		{
			BEE_RAISE(GLFatal, "Failed to create shader program.");
		}
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
	Shader &attach(GLuint shaderPiece)
	{
		glAttachShader(shader, shaderPiece);
		return *this;
	}
private:
	GLuint shader;
};

}

}