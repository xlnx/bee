#pragma once

#include "common.h"
#include <vector>
#include <sstream>

namespace bee
{

namespace gl
{

enum TextureDim { 
	Tex1D = GL_TEXTURE_1D, 
	Tex2D = GL_TEXTURE_2D, 
	Tex3D = GL_TEXTURE_CUBE_MAP
};

class TextureBase;

template <TextureDim Dim>
class Texture;

class TextureGenerator
{
	friend class TextureBase;
	constexpr static std::size_t initSize = 4;
public:
	TextureGenerator()
	{
		ptr = dataptr = new GLuint [initSize];
		dataend = dataptr + initSize;
		if (haveInstance)
		{
			BEE_RAISE(GLFatal, "Cannot create SINGLETON class bee::gl::TextureGenerator.");
		}
		else
		{
			haveInstance = true;
		}
		glGenTextures(initSize, dataptr);
	}
	TextureGenerator(const TextureGenerator &) = delete;
	~TextureGenerator()
	{
		delete [] dataptr;
	}
private:
	static GLuint gen()
	{
		if (ptr != dataend)
		{
			// glBindTexture(Ty, *ptr);
			return *ptr++;
		}
		else
		{
			::std::size_t size = (dataend - dataptr) << 1;
			delete [] dataptr;
			ptr = dataptr = new GLuint [size];
			dataend = dataptr + size;
			glGenTextures(size, dataptr);
			// glBindTexture(Ty, *ptr);
			return *ptr++;
		}
	}
private:
	static bool haveInstance;
	static GLuint *dataptr;
	static GLuint *dataend;
	static GLuint *ptr;
};

class TextureBase
{
protected:
	TextureBase(bool isValid = false): isValid(isValid)
	{
	}
public:
	operator GLuint () const
	{
		return handle;
	}
	bool valid() const
	{
		return isValid;
	}
protected:
	static unsigned char *loadImage(::std::string path, int &width, int &height, int &comp);
	static void freeImage(unsigned char *data);
protected:
	GLuint handle = TextureGenerator::gen();
	bool isValid = false;
};

template <TextureDim Dim>
class TextureNDBase: public TextureBase
{
protected:
	TextureNDBase(bool isValid = false): TextureBase(isValid)
	{
	}
public:
	void invoke(int i) const
	{
		glActiveTexture(GL_TEXTURE0 + i);
		bind();
	}
protected:
	void bind() const 
	{
		glBindTexture(Dim, handle);
	}
	void unbind() const 
	{
		glBindTexture(Dim, 0);
	}
};

template <>
class Texture<Tex1D>: public TextureNDBase<Tex1D>
{
public:
	Texture() = default;
	Texture(const std::vector<::glm::vec3> &points): 
		TextureNDBase<Tex1D>(true)
	{
		glBindTexture(Tex1D, handle);
			glTexImage1D(Tex1D, 0, GL_RGB, points.size(), 0.f, GL_RGB, GL_FLOAT, &points[0]);
			glTexParameterf(Tex1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(Tex1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(Tex1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(Tex1D, 0);
	}
};

class RandomTexture: public Texture<Tex1D>
{
public:
	RandomTexture() = default;
	RandomTexture(::std::size_t n): 
		Texture<Tex1D>(getRandomVector(n))
	{
	}
private:
	static ::std::vector<::glm::vec3> getRandomVector(::std::size_t n)
	{
		static const auto randMax = []() ->int { srand(time(nullptr)); return RAND_MAX; } ();
		::std::vector<::glm::vec3> vec(n);
		for (auto &e: vec)
		{
			e = { float(rand()) / randMax, float(rand()) / randMax, float(rand()) / randMax };
		}
		return vec;
	}
};

template <>
class Texture<Tex2D>: public TextureNDBase<Tex2D>
{
public:
	Texture() = default;
	Texture(const ::std::string &path, bool useMipmap = true): 
		TextureNDBase<Tex2D>(true)
	{
		int width, height, componentCount;
		auto data = loadImage(path, width, height, componentCount);
		if (!data)
		{
			BEE_RAISE(Fatal, "failed to load image: " + path);
		}
		auto format = componentCount == 1 ? GL_RED : 
			componentCount == 3 ? GL_RGB : 
			componentCount == 4 ? GL_RGBA : 0;
		if (!format)
		{
			BEE_RAISE(Fatal, "invalid image format.");
		}
		bind();
			glTexImage2D(Tex2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glTexParameteri(Tex2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(Tex2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(Tex2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (useMipmap)
			{
				glTexParameteri(Tex2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGenerateMipmap(Tex2D);
			}
			else
			{
				glTexParameteri(Tex2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		unbind();
		freeImage(data);
	}
};

template <>
class Texture<Tex3D>: public TextureNDBase<Tex3D>
{
public:
	Texture(::std::string path)
	{
		bind();
		char *p = const_cast<char *>(&path[0]), *q = p;
		while (*p != 0)
		{
			if (*p == '.') q = p;
			p++;
		}
		::std::string ext = q;
		path = path.substr(0, q - &path[0]);
		for (auto i = 0u; i != 6; ++i)
		{
			int width, height, componentCount;
			static const char *const suffix[] = {
				"_right", "_left", "_front", "_back", "_top", "_bottom"
			};
			::std::ostringstream os;
			os << path << suffix[i] << ext;
			if (auto data = loadImage(os.str(), width, height, componentCount))
			{
				auto format = componentCount == 1 ? GL_RED : 
					componentCount == 3 ? GL_RGB : 
					componentCount == 4 ? GL_RGBA : 0;
				if (!format)
				{
					BEE_RAISE(Fatal, "invalid image format.");
				}
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				
				freeImage(data);
			}
		}
		glTexParameteri(Tex3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Tex3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		unbind();
	}
};

using Texture1D = Texture<Tex1D>;
using Texture2D = Texture<Tex2D>;
using Texture3D = Texture<Tex3D>;

}

}