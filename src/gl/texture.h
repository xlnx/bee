#pragma once

#include "common.h"

namespace bee
{

namespace gl
{

enum TextureDim { 
	Tex1D = GL_TEXTURE_1D, 
	Tex2D = GL_TEXTURE_2D, 
	Tex3D = GL_TEXTURE_CUBE_MAP
};

template <TextureDim Dim>
class Texture;

class TextureGenerator
{
	friend class Texture<Tex1D>;
	friend class Texture<Tex2D>;
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
	TextureBase() = default;
	TextureBase(GLuint handle):
		handle(handle)
	{
	}
public:
	operator GLuint () const
	{
		return handle;
	}
	operator bool () const
	{
		return ~handle;
	}
protected:
	static unsigned char *loadImage(::std::string path, int &width, int &height, int &comp);
	static void freeImage(unsigned char *data);
protected:
	GLuint handle = -1;
};

template <TextureDim Dim>
class Texture: public TextureBase
{
public:
	Texture() = default;
	Texture(const ::std::string &path, bool useMipmap = true): TextureBase(TextureGenerator::gen())
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
		glBindTexture(Dim, handle);
			glTexImage2D(Dim, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glTexParameteri(Dim, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(Dim, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(Dim, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (useMipmap)
			{
				glTexParameteri(Dim, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGenerateMipmap(Dim);
			}
			else
			{
				glTexParameteri(Dim, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		glBindTexture(Dim, 0);
		freeImage(data);
	}
	void invoke(int i) const
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(Dim, handle);
	}
};

// template <>
// class Texture<Tex3D>: TextureBase
// {
// public:
// 	Texture(const ::std::string &path): TextureBase(TextureGenerator::gen())
// 	{
// 		glBindTexture(Tex3D, handle);
// 		for (auto i = 0u; i != faces.size();++i)
// 		{
// 			int width, height, componentCount;
// 			auto data = stbi_load(faces[i].c_str(), &width, &height, &componentCount, 0);
// 			if (!data)
// 			{
// 				BEE_RAISE(Fatal, "failed to load image: " + faces[i]);
// 			}
// 			auto format = componentCount == 1 ? GL_RED : 
// 				componentCount == 3 ? GL_RGB : 
// 				componentCount == 4 ? GL_RGBA : 0;
// 			if (!format)
// 			{
// 				BEE_RAISE(Fatal, "invalid image format.");
// 			}
// 			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
// 			stbi_image_free(data);
// 		}
// 		glTexParameteri(Tex3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 		glTexParameteri(Tex3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// 	}
// };

}

}