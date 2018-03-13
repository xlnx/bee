#pragma once 

#include "common.h"
#include "property.h"
#include <cstdlib>
#include <map>
#include <vector>
#include <stack>
#include <functional>
#include <initializer_list>

namespace bee
{

namespace gl
{

template <int ShaderType> class ShaderObj;

class Shader;

template <typename T> struct UniformRef;

class ShaderControllers;




template <int ShaderType>
	class ShaderObj final
{
	friend class Shader;
public:
	ShaderObj(std::string filename):
		shader(glCreateShader(ShaderType))
	{
		filename = getShaderPath() + filename;
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
	static ::std::string &getShaderPath()
	{
		static ::std::string shaderPath;
		return shaderPath;
	}
private:
	GLuint shader;
};

using VertexShader = ShaderObj<GL_VERTEX_SHADER>;
using GeometryShader = ShaderObj<GL_GEOMETRY_SHADER>;
using FragmentShader = ShaderObj<GL_FRAGMENT_SHADER>;

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
	static GLint getUniforms(int index);
	static ::std::pair<GLint, GLint> &getUniformArrays(int index);
protected:
	int index = 0;
};

template <typename T>
	struct UniformRefManip
{
	void set(GLint, const T &) const {}
};

template <typename T>
	struct UniformRef: UniformRefManip<T>, UniformRefBase
{
	bool operator = (const T &target) const
	{
		auto handle = getUniforms(index);
		UniformRefManip<T>::set(handle, target);
		return ~handle;
	}
};

template <typename T>
	struct UniformRefElem: UniformRefManip<T>, UniformRefBase
{
	bool operator = (const T &target) const
	{
		UniformRefManip<T>::set(index, target);
		return ~index;
	}
};

template <>
	struct UniformRefManip<::glm::mat2>
{
	void set(GLint handle, const ::glm::mat2 &target) const
	{
		glUniformMatrix2fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRefManip<::glm::mat3>
{
	void set(GLint handle, const ::glm::mat3 &target) const
	{
		glUniformMatrix3fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRefManip<::glm::mat4>
{
	void set(GLint handle, const ::glm::mat4 &target) const
	{
		glUniformMatrix4fv(handle, 1, GL_TRUE, reinterpret_cast<const float*>(&target));
	}
};

template <>
	struct UniformRefManip<int>
{
	void set(GLint handle, const int &target) const
	{
		glUniform1i(handle, target);
	}
};

template <>
	struct UniformRefManip<float>
{
	void set(GLint handle, const float &target) const
	{
		glUniform1f(handle, target);
	}
};

template <>
	struct UniformRefManip<::glm::vec2>
{
	void set(GLint handle, const ::glm::vec2 &target) const
	{
		glUniform2f(handle, target[0], target[1]);
	}
};

template <>
	struct UniformRefManip<::glm::vec3>
{
	void set(GLint handle, const ::glm::vec3 &target) const
	{
		glUniform3f(handle, target[0], target[1], target[2]);
	}
};

template <>
	struct UniformRefManip<::glm::vec4>
{
	void set(GLint handle, const ::glm::vec4 &target) const
	{
		glUniform4f(handle, target[0], target[1], target[2], target[3]);
	}
};

struct ShaderControllerInfo
{
	gl::UniformRef<int> counter;
	const char *typeName;
};
using ShaderControllerInfoGetter = ShaderControllerInfo *(*)();

class ShaderController
{
protected:
	ShaderController(const ::std::string &prefix): 
		prefix(prefix)
	{
	}
public:
	virtual ~ShaderController() = default;
	
	virtual ShaderControllerInfoGetter getInfoFunc() = 0;
public:
	virtual bool invoke(int index) = 0;
private:
	const ::std::string prefix = "ShaderController";
};

// template <typename T = ShaderController, typename = typename 
// 	::std::enable_if<::std::is_base_of<ShaderController, T>::value>::type>
class ShaderControllers final
{
	friend class Shader;
public:
	void invoke()
	{
		::std::map<ShaderControllerInfo*, int> ptrs;
		for (auto & controller: controllers)
		{
			auto info = controller->getInfoFunc()();
			if (controller->invoke(ptrs[info]))
			{
				++ptrs[info];
			}
		}
		for (auto &pair: ptrs)
		{
			pair.first->counter = pair.second;
		}
	}
	void addController(ShaderController &controller)
	{
		controllers.push_back(&controller);
	}
	void foreach(const ::std::function<void(ShaderController*)> &f)
	{
		for (auto controller: controllers)
		{
			f(controller);
		}
	}
	static ShaderControllers &getControllers()
	{
		return *getCurrent();
	}
	static void setControllers(ShaderControllers &current)
	{
		getCurrent() = &current;
	}
private:
	static ShaderControllers *&getCurrent()
	{
		static ShaderControllers *current = nullptr;
		return current;
	}
protected:
	::std::vector<ShaderController *> controllers;
};

class Shader final
{
	static constexpr auto maxUniformCount = 512;
	struct ShaderRec
	{
		Shader *shader;
		ShaderRec *next = nullptr, *prev = nullptr;
	};
	friend class UniformRefBase;
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
		init();
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
		init();
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
		if (!bindShader)
		{
			glUseProgram(shader);
			getCurrShader() = this;
			getShaders().push(this);
			if (auto controllers = ShaderControllers::getCurrent())
			{
				controllers->invoke();
			}
		}
	}
	void unuse()
	{
		if (!bindShader)
		{
			if (!getShaders().empty())
			{
				auto top = getShaders().top();
				getShaders().pop();
				getCurrShader() = top;
				glUseProgram(*top);
			}
			else
			{
				getCurrShader() = nullptr;
				glUseProgram(0);
			}
		}
	}
	template <typename ...Types>
	void setTransformFeedbackVaryings(Types ...varyings)
	{
		const char *data[] = { varyings... };
		glTransformFeedbackVaryings(shader, sizeof...(Types), data, GL_INTERLEAVED_ATTRIBS);
		link();
	}
	void setTransformFeedbackVaryings(const ::std::vector<const char*> &varyings)
	{
		glTransformFeedbackVaryings(shader, varyings.size(), &*varyings.begin(), GL_INTERLEAVED_ATTRIBS);
		link();
	}
	void setTransformFeedbackVaryings(const ::std::initializer_list<const char*> &varyings)
	{
		glTransformFeedbackVaryings(shader, varyings.size(), &*varyings.begin(), GL_INTERLEAVED_ATTRIBS);
		link();
	}
public:
	static void bind(Shader &shader)
	{
		if (!bindShader)
		{
			shader.use();
			bindShader = &shader;
		}
	}
	static void unbind()
	{
		if (bindShader)
		{
			bindShader->unuse();
			bindShader = nullptr;
		}
	}
	static void setFilePath(const char *path)
	{
		VertexShader::getShaderPath() = path;
		GeometryShader::getShaderPath() = path;
		FragmentShader::getShaderPath() = path;
	}
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
	static void initialize()
	{
		uniformIndex = new ::std::map<::std::string, int>();
		registeredUniforms = new ::std::vector<::std::string>();
		uniformArrayIndex = new ::std::map<::std::string, int>();
		registeredUniformArrays = new ::std::vector<::std::pair<::std::string, ::std::string>>();
	}
	template <typename ...Types>
	static Shader &load(const ::std::string &name, Types &&...args)
	{
		auto &loaded = getLoaded();
		if (auto shader = loaded[name])
		{
			return *shader;
		}
		else
		{
			return *(loaded[name] = new Shader(::std::forward<Types>(args)...));
		}
	}
	template <typename ...Types>
	static Shader &loadTransformFeedback(const ::std::string &name, 
		const ::std::initializer_list<::std::string> &l, Types &&...args)
	{
		auto &loaded = getLoaded();
		if (auto shader = loaded[name])
		{
			return *shader;
		}
		else
		{
			shader = new Shader(::std::forward<Types>(args)...);
			shader->setTransformFeedbackVaryings(l);
			return *(loaded[name] = shader);
		}
	}
private:
	static ::std::map<::std::string, Shader*> &getLoaded()
	{
		static ::std::map<::std::string, Shader*> loaded;
		return loaded;
	}
	static Shader *&getCurrShader()
	{
		static Shader *currShader = nullptr;
		return currShader;
	}
	static ::std::stack<Shader *> getShaders()
	{
		static ::std::stack<Shader *> shaders;
		return shaders;
	}
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
	void link()
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
		// glValidateProgram(shader);
		// glGetProgramiv(shader, GL_VALIDATE_STATUS, &success);
		// if (!success)
		// {
		// 	glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		// 	std::ostringstream os;
		// 	os << "Invalid shader program." << std::endl << infoLog;
		// 	BEE_RAISE(GLFatal, os.str());
		// }
	}
	void init()
	{
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
private:
	GLuint shader;
	ShaderRec *prec;
	GLuint *uniforms;
	::std::pair<GLint, GLint> *uniformArrays;
private:
	static Shader *bindShader;
	static ShaderRec shaderHead;
	// ::std::map<::std::string, GLuint> bufferedUniforms;
	static ::std::map<::std::string, int> *uniformIndex;
	static ::std::vector<::std::string> *registeredUniforms;

	static ::std::map<::std::string, int> *uniformArrayIndex;
	static ::std::vector<::std::pair<::std::string, ::std::string>> *registeredUniformArrays;
};

inline GLint UniformRefBase::getUniforms(int index)
{
	return Shader::getCurrShader()->uniforms[index];
}

inline ::std::pair<GLint, GLint> &UniformRefBase::getUniformArrays(int index)
{
	return Shader::getCurrShader()->uniformArrays[index];
}

template <typename T>
	struct UniformRef<T[]>: UniformRefBase
{
	UniformRefElem<T> operator [] (int index) const
	{
		UniformRefElem<T> ref;
		auto &data = getUniformArrays(this->index);
		int handle = ~data.first ? data.first + data.second * index : -1;
		reinterpret_cast<int&>(ref) = handle;
		return ref;
	}
};

}

}
