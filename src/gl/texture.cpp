#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace bee
{

namespace gl
{

bool TextureGenerator::haveInstance = false;
GLuint *TextureGenerator::dataptr = nullptr;
GLuint *TextureGenerator::dataend = nullptr;
GLuint *TextureGenerator::ptr = nullptr;

unsigned char *TextureBase::loadImage(::std::string path, int &width, int &height, int &comp)
{
	// if the path is wrapped by quotes
	if (path[0] == '"')
	{
		path = path.substr(1, path.length() - 2);
	}
	return stbi_load(path.c_str(), &width, &height, &comp, 0);
}

void TextureBase::freeImage(unsigned char *data)
{
	stbi_image_free(data);
}

}

}