#pragma once

#include "common.h"
#include "texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace bee
{

namespace gl
{

constexpr auto Diffuse = aiTextureType_DIFFUSE;
constexpr auto Specular = aiTextureType_SPECULAR;
constexpr auto Ambient = aiTextureType_AMBIENT;
constexpr auto Emissive = aiTextureType_EMISSIVE;
constexpr auto Normals = aiTextureType_NORMALS;

constexpr int TextureTypeSize = Normals + 1;

class Material
{
public:
	template <aiTextureType type>
	const Texture<Tex2D> &getTexture() const
	{
		return textures[type];
	}
	template <aiTextureType Type>
	void addTexture(const ::std::string &path, bool useMipmap = true)
	{
		textures[Type] = Texture<Tex2D>(path, useMipmap);
	}
protected:
	Texture<Tex2D> textures[TextureTypeSize];
};

}

}