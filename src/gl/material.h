#pragma once

#include "common.h"
#include "texture.h"
#include "shader.h"
#include "configure.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <map>

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
	BEE_UNIFORM(float, SpecularIntensity, "Material");
	BEE_UNIFORM(float, SpecularPower, "Material");
	BEE_UNIFORM(float, DiffuseIntensity, "Material");
public:
	Material(::std::string name = "")
	{
		textureControllers[Diffuse] = gl::Shader::uniform<int>("gMaterial.Diffuse");
		textureControllers[Specular] = gl::Shader::uniform<int>("gMaterial.Specular");
		textureControllers[Ambient] = gl::Shader::uniform<int>("gMaterial.Ambient");
		textureControllers[Emissive] = gl::Shader::uniform<int>("gMaterial.Emissive");
		textureControllers[Normals] = gl::Shader::uniform<int>("gMaterial.Normals");
		
		if (name != "")
		{
			name = getFilePath() + name + "/";
			Configure cfg(name + "config.json");
			{
				auto texture = cfg["texture"];
				if (texture.isObject())
				{
					auto normal = texture["normal"];
					if (normal.isString())
					{
						textures[Normals] = Texture2D(name + normal.asString(), false);
					}
				}
			}
			{
				auto specular = cfg["specular"];
				if (specular.isObject())
				{
					auto power = specular["power"];
					if (power.isDouble())
					{
						fSpecularPower = power.asDouble();
					}
					auto intensity = specular["intensity"];
					if (intensity.isDouble())
					{
						fSpecularIntensity = intensity.asDouble();
					}
				}

				auto diffuse = cfg["diffuse"];
				if (diffuse.isObject())
				{
					auto intensity = diffuse["intensity"];
					if (intensity.isDouble())
					{
						fDiffuseIntensity = intensity.asDouble();
					}
				}
			}
		}
	}
	
	template <aiTextureType Type>
	const Texture2D &getTexture()
	{
		return textures[Type];
	}
	template <aiTextureType Type>
	void addTexture(const ::std::string &path, bool useMipmap = true)
	{
		textures[Type] = Texture2D(path, useMipmap);
	}
	void use() const
	{
		for (auto &texture: textures)
		{
			texture.second.invoke(texture.first);
			textureControllers[texture.first] = texture.first;
		}
		gSpecularPower = fSpecularPower;
		gSpecularIntensity = fSpecularIntensity;
		gDiffuseIntensity = fDiffuseIntensity;
	}
	static void setFilePath(const ::std::string &path)
	{
		getPathW() = path;
	}
	static const ::std::string &getFilePath()
	{
		return getPathW();
	}
public:
	static ::std::string &getPathW()
	{
		static ::std::string path;
		return path; 
	}
protected:
	// Texture2D textures[TextureTypeSize];
	::std::map<aiTextureType, Texture2D> textures;
	UniformRef<int> textureControllers[TextureTypeSize];
public:
	BEE_PROPERTY(float, SpecularPower) = .5f;
	BEE_PROPERTY(float, SpecularIntensity) = 1.f;
	BEE_PROPERTY(float, DiffuseIntensity) = .5f;
};

}

}