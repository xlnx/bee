#pragma once 

#include "common.h"
#include <cstdlib>
#include <map>
#include <vector>

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

class UniformRefBase
{
protected:
	friend class Shader;
	UniformRefBase() = default;
public:
	void bind(const UniformRefBase &other)
	{
		index = other.index;
	}
protected:
	int index = 0;
};

class UniformRefHack;

class Shader final
{
	static constexpr auto maxUniformCount = 512;
	struct ShaderRec
	{
		Shader *shader;
		ShaderRec *next = nullptr, *prev = nullptr;
	};
	friend class UniformRefHack;
public:
	template <typename ...Types>
	Shader(Types &&...args):
		shader(glCreateProgram()), 
		prec(new ShaderRec{this, shaderHead.next, &shaderHead}),
		uniforms(new GLuint [maxUniformCount])
	{
		if (!shader)
		{
			BEE_RAISE(GLFatal, "Failed to create shader program.");
		}
		attach(std::forward<Types>(args)...);
		link();
		memset(uniforms, -1, sizeof(*uniforms) * maxUniformCount);
		for (auto i = 0u; i != registeredUniforms.size(); ++i)
		{
			uniforms[i + 1] = glGetUniformLocation(shader, registeredUniforms[i].c_str());
		}
		if (shaderHead.next)
		{
			shaderHead.next->prev = prec;
		}
		shaderHead.next = prec;
	}
	template <template <typename T, typename _Alloc> class E,
		typename Alloc>
	Shader(const E<GLuint, Alloc> &v):
		shader(glCreateProgram()), 
		prec(new ShaderRec{this, shaderHead.next, &shaderHead}),
		uniforms(new GLuint [maxUniformCount])
	{
		for (auto e: v) attach(e);
		link();
		memset(uniforms, -1, sizeof(*uniforms) * maxUniformCount);
		for (int i = 0; i != registeredUniforms.size(); ++i)
		{
			uniforms[i + 1] = glGetUniformLocation(shader, registeredUniforms[i].c_str());
		}
		if (shaderHead.next)
		{
			shaderHead.next->prev = prec;
		}
		shaderHead.next = prec;
	}
	Shader(const Shader &) = delete;
	Shader(Shader &&other):
		shader(other.shader), 
		prec(other.prec),
		uniforms(other.uniforms)
	{
		prec->shader = this;
		other.shader = 0;
		other.prec = nullptr;
	}
	~Shader()
	{
		if (prec)
		{
			glDeleteProgram(shader);
			if (prec->prev->next = prec->next)
			{
				prec->next->prev = prec->prev;
			}
			delete prec;
			delete [] uniforms;
		}
	}
	
	operator GLuint() const
	{
		return shader;
	}
	void use()
	{
		glUseProgram(shader);
		currShader = this;
	}
public:
	template <typename T>
		static UniformRef<T> uniform(const ::std::string &name)
	{
		UniformRef<T> ref;
		auto iter = uniformIndex.find(name);
		if (iter != uniformIndex.end())
		{
			reinterpret_cast<UniformRefBase&>(ref).index = iter->second;
		}
		else
		{
			registeredUniforms.emplace_back(name);
			auto index = uniformIndex[name] = registeredUniforms.size();
			reinterpret_cast<UniformRefBase&>(ref).index = index;
			for (auto ptr = shaderHead.next; ptr != nullptr; ptr = ptr->next)
			{
				ptr->shader->uniforms[index] = glGetUniformLocation(*ptr->shader, name.c_str());
			}
		}
		return ref;
	}
	static Shader &current()
	{
		return *currShader;
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
	ShaderRec *prec;
	GLuint *uniforms;
private:
	static Shader *currShader;
	static ShaderRec shaderHead;
	// ::std::map<::std::string, GLuint> bufferedUniforms;
	static ::std::map<::std::string, int> uniformIndex;
	static ::std::vector<::std::string> registeredUniforms;
};

class UniformRefHack: public UniformRefBase
{
protected:
	GLuint handle() const
	{
		return Shader::current().uniforms[UniformRefBase::index];
	}
};

template <typename T>
	struct UniformRef: UniformRefHack
{
	bool operator = (const T &target) const;
};

template <>
	struct UniformRef<::glm::mat2>: UniformRefHack
{
	bool operator = (const ::glm::mat2 &target) const
	{
		auto targ = handle();
		glUniformMatrix2fv(targ, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
		return ~targ;
	}
};

template <>
	struct UniformRef<::glm::mat3>: UniformRefHack
{
	bool operator = (const ::glm::mat3 &target) const
	{
		auto targ = handle();
		glUniformMatrix3fv(targ, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
		return ~targ;
	}
};

template <>
	struct UniformRef<::glm::mat4>: UniformRefHack
{
	bool operator = (const ::glm::mat4 &target) const
	{
		auto targ = handle();
		glUniformMatrix4fv(targ, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
		return ~targ;
	}
};

template <>
	struct UniformRef<int>: UniformRefHack
{
	bool operator = (const int &target) const
	{
		auto targ = handle();
		glUniform1i(targ, target);
		return ~targ;
	}
};

template <>
	struct UniformRef<float>: UniformRefHack
{
	bool operator = (const float &target) const
	{
		auto targ = handle();
		glUniform1f(targ, target);
		return ~targ;
	}
};

template <>
	struct UniformRef<::glm::vec2>: UniformRefHack
{
	bool operator = (const ::glm::vec2 &target) const
	{
		auto targ = handle();
		glUniform2f(targ, target[0], target[1]);
		return ~targ;
	}
};

template <>
	struct UniformRef<::glm::vec3>: UniformRefHack
{
	bool operator = (const ::glm::vec3 &target) const
	{
		auto targ = handle();
		glUniform3f(targ, target[0], target[1], target[2]);
		return ~targ;
	}
};

template <>
	struct UniformRef<::glm::vec4>: UniformRefHack
{
	bool operator = (const ::glm::vec4 &target) const
	{
		auto targ = handle();
		glUniform4f(targ, target[0], target[1], target[2], target[3]);
		return ~targ;
	}
};

}

}