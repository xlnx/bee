#pragma once

#include "common.h"
#include "material.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace bee
{

namespace gl
{

class ModelBase
{
public:
	virtual ~ModelBase() = default;
	virtual void render() const = 0;
};

constexpr auto assImportFlags = 
	aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
	aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
	aiProcess_CalcTangentSpace;

class Model: private ModelBase
{
public:
	Model() = default;
	Model(::std::string file)
	{
		file = modelPath() + file;
		char prevPath[2048];
#		ifdef WIN32
		if (!GetCurrentDirectory(sizeof(prevPath), prevPath))
#		else
		if (!getcwd(prevPath, sizeof(prevPath)))
#		endif
		{
			BEE_RAISE(Fatal, "Unable to get current working directory.");
		}
		::std::string path = file;
		char *ptr = const_cast<char*>(&path[0]) + path.length() - 1;
		char *shortName = const_cast<char*>(&path[0]);
		for (; ptr >= &path[0]; --ptr)
		{
			if (*ptr == '/' || *ptr == '\\')
			{
				*ptr = 0;
				shortName = ptr + 1;
				break;
			}
		}
#		ifdef WIN32
		SetCurrentDirectory(path.c_str());
#		else
		setcwd(path.c_str());
#		endif

		Assimp::Importer ass;
		scene = ass.ReadFile(shortName, assImportFlags);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			BEE_RAISE(GLFatal, "Failed to import model(" + file + "). Assimp: " + ass.GetErrorString());
		}
		// BEE_LOG(root->mNumChildren);
		// for (auto i = 0u; i != root->mNumChildren; ++i)
		// {

		// }
		materials.resize(scene->mNumMaterials);
		for (auto i = 0u; i != scene->mNumMaterials; ++i)
		{
			auto material = scene->mMaterials[i];
			if (material->GetTextureCount(Diffuse) > 0)
			{
				BEE_LOG("Diffuse");
				aiString path;
				material->GetTexture(Diffuse, 0, &path);
				materials[i].addTexture<Diffuse>(path.C_Str());
			}
			if (material->GetTextureCount(Specular) > 0)
			{
				BEE_LOG("Specular");
				aiString path;
				material->GetTexture(Specular, 0, &path);
				materials[i].addTexture<Specular>(path.C_Str());
			}
			if (material->GetTextureCount(Ambient) > 0)
			{
				BEE_LOG("Ambient");
				aiString path;
				material->GetTexture(Ambient, 0, &path);
				materials[i].addTexture<Ambient>(path.C_Str());
			}
			if (material->GetTextureCount(Emissive) > 0)
			{
				BEE_LOG("Emissive");
				aiString path;
				material->GetTexture(Emissive, 0, &path);
				materials[i].addTexture<Emissive>(path.C_Str());
			}
			if (material->GetTextureCount(Normals) > 0)
			{
				BEE_LOG("Normals");
				aiString path;
				material->GetTexture(Normals, 0, &path);
				materials[i].addTexture<Normals>(path.C_Str());
			}
		}
		auto root = scene->mRootNode;
		for (auto i = 0u; i != scene->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[i];
			meshes.emplace_back(&materials[mesh->mMaterialIndex], mesh);
		}
#		ifdef WIN32
		SetCurrentDirectory(prevPath);
#		else
		setcwd(prevPath);
#		endif
	}

	void render() const override
	{
		for (auto &mesh: meshes) mesh.render();
	}
	static void setFilePath(const ::std::string &path)
	{
		modelPath() = path;
	}
private:
	static ::std::string &modelPath()
	{
		static ::std::string path = "";
		return path;
	}
protected:
	::std::vector<Material> materials;
	::std::vector<Mesh> meshes;
	const aiScene *scene = nullptr;
};

}

}