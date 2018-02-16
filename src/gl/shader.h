#pragma once 

#include "common.h"
#include "property.h"
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

class UniformRefHack;

class UniformRefBase
{
protected:
	friend class Shader;
	friend class UniformRefHack;
	UniformRefBase() = default;
public:
	void bind(const UniformRefBase &other)
	{
		index = other.index;
	}
protected:
	int index = 0;
};

class UniformRefHack: public UniformRefBase
{
protected:
	GLuint handle() const;
	const ::std::pair<GLint, GLint> *getHack() const;
	template <typename T>
	void hackLocation(UniformRef<T> &ref, int location) const
	{
		reinterpret_cast<UniformRefBase&>(ref).index = -location;
	}
};

template <typename T>
	struct UniformRef: UniformRefHack
{
	bool operator = (const T &target) const;
};

template <typename T>
	struct UniformRef<T[]>: UniformRefHack
{
	UniformRef<T> operator [] (int index) const
	{
		UniformRef<T> ref;
		auto & data = *getHack();
		auto location = data.first + data.second * index;
		hackLocation(ref, location);
		return ref;
	}
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

class Shader final
{
	static constexpr auto maxUniformCount = 512;
	struct ShaderRec
	{
		Shader *shader;
		ShaderRec *next = nullptr, *prev = nullptr;
	};
	friend class UniformRefHack;
	friend class GLWindowBase;
public:
	template <typename ...Types>
	Shader(Types &&...args):
		shader(glCreateProgram()), 
		prec(new ShaderRec{this, shaderHead.next, &shaderHead}),
		uniforms(new GLuint [maxUniformCount]),
		uniformArrays(new ::std::pair<GLint, GLint> [maxUniformCount])
	{
		if (!shader)
		{
			BEE_RAISE(GLFatal, "Failed to create shader program.");
		}
		attach(std::forward<Types>(args)...);
		link();
		memset(uniforms, -1, sizeof(*uniforms) * maxUniformCount);
		for (auto i = 0u; i != registeredUniforms->size(); ++i)
		{
			uniforms[i + 1] = glGetUniformLocation(shader, registeredUniforms->operator[](i).c_str());
		}
		for (auto i = 0u; i != registeredUniformArrays->size(); ++i)
		{
			auto first = glGetUniformLocation(shader, registeredUniformArrays->operator[](i).first.c_str());
			auto second = glGetUniformLocation(shader, registeredUniformArrays->operator[](i).second.c_str());
			uniformArrays[i + 1] = ::std::make_pair(first, second - first);
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
		uniforms(new GLuint [maxUniformCount]),
		uniformArrays(new ::std::pair<GLint, GLint> [maxUniformCount])
	{
		for (auto e: v) attach(e);
		link();
		memset(uniforms, -1, sizeof(*uniforms) * maxUniformCount);
		for (int i = 0; i != registeredUniforms->size(); ++i)
		{
			uniforms[i + 1] = glGetUniformLocation(shader, registeredUniforms->operator[](i).c_str());
		}
		for (auto i = 0u; i != registeredUniformArrays->size(); ++i)
		{
			auto first = glGetUniformLocation(shader, registeredUniformArrays->operator[](i).first.c_str());
			auto second = glGetUniformLocation(shader, registeredUniformArrays->operator[](i).second.c_str());
			uniformArrays[i + 1] = ::std::make_pair(first, second - first);
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
		uniforms(other.uniforms),
		uniformArrays(other.uniformArrays)
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
			delete [] uniformArrays;
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
		gTime = float(glfwGetTime());
	}
public:
	template <typename T>
		static UniformRef<T> uniform(const ::std::string &name)
	{
		if (auto p = strstr(name.c_str(), "[]"))
		{
			UniformRef<T> ref;
			auto iter = uniformArrayIndex->find(name);
			if (iter != uniformArrayIndex->end())
			{
				reinterpret_cast<UniformRefBase&>(ref).index = iter->second;
			}
			else
			{
				auto front = name.substr(0, p - &name[0] + 1);
				auto back = name.substr(p - &name[0] + 1);
				auto name0 = front + "0" + back;
				auto name1 = front + "1" + back;

				registeredUniformArrays->emplace_back(name0, name1);
				auto index = uniformArrayIndex->operator[](name) = registeredUniformArrays->size();
				
				reinterpret_cast<UniformRefBase&>(ref).index = index;
				for (auto ptr = shaderHead.next; ptr != nullptr; ptr = ptr->next)
				{
					auto first = glGetUniformLocation(*ptr->shader, name0.c_str());
					auto second = glGetUniformLocation(*ptr->shader, name1.c_str());
					ptr->shader->uniformArrays[index] = ::std::make_pair(first, second - first);
				}
			}
			return ref;
		}
		else
		{
			UniformRef<T> ref;
			auto iter = uniformIndex->find(name);
			if (iter != uniformIndex->end())
			{
				reinterpret_cast<UniformRefBase&>(ref).index = iter->second;
			}
			else
			{
				registeredUniforms->emplace_back(name);
				auto index = registeredUniforms->size();
				uniformIndex->operator[](name) = index;
				reinterpret_cast<UniformRefBase&>(ref).index = index;
				for (auto ptr = shaderHead.next; ptr != nullptr; ptr = ptr->next)
				{
					ptr->shader->uniforms[index] = glGetUniformLocation(*ptr->shader, name.c_str());
				}
			}
			return ref;
		}
	}
	static Shader &current()
	{
		return *currShader;
	}
	static void initialize()
	{
		uniformIndex = new ::std::map<::std::string, int>();
		registeredUniforms = new ::std::vector<::std::string>();
		uniformArrayIndex = new ::std::map<::std::string, int>();
		registeredUniformArrays = new ::std::vector<::std::pair<::std::string, ::std::string>>();
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
		if (!success)
		{
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
	::std::pair<GLint, GLint> *uniformArrays;
	BEE_UNIFORM_GLOB(float, Time);
private:
	static Shader *currShader;
	static ShaderRec shaderHead;
	// ::std::map<::std::string, GLuint> bufferedUniforms;
	static ::std::map<::std::string, int> *uniformIndex;
	static ::std::vector<::std::string> *registeredUniforms;

	static ::std::map<::std::string, int> *uniformArrayIndex;
	static ::std::vector<::std::pair<::std::string, ::std::string>> *registeredUniformArrays;
};

inline GLuint UniformRefHack::handle() const
{
	if (UniformRefBase::index > 0)
	{
		return Shader::current().uniforms[UniformRefBase::index];
	}
	else
	{
		return -UniformRefBase::index;
	}
}

inline const ::std::pair<GLint, GLint> *UniformRefHack::getHack() const
{
	return Shader::current().uniformArrays + UniformRefBase::index;
}

}

}
