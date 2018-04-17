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
		chdir(path.c_str());
#		endif

		Assimp::Importer ass;
		auto scene = ass.ReadFile(shortName, assImportFlags);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			BEE_RAISE(Fatal, "Failed to import model(" + file + "). Assimp: " + ass.GetErrorString());
		}
		// BEE_LOG(root->mNumChildren);
		// for (auto i = 0u; i != root->mNumChildren; ++i)
		// {

		// }
		// materials.resize(scene->mNumMaterials);
		for (auto i = 0u; i != scene->mNumMaterials; ++i)
		{
			materials.emplace_back(scene->mMaterials[i]);
		}
		// auto root = scene->mRootNode;
		for (auto i = 0u; i != scene->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[i];
			meshes.emplace_back(&materials[mesh->mMaterialIndex], mesh);
		}
#		ifdef WIN32
		SetCurrentDirectory(prevPath);
#		else
		chdir(prevPath);
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
	static gl::Model &load(const ::std::string &name)
	{
		static ::std::map<::std::string, gl::Model*> loaded;
		if (auto model = loaded[name])
		{
			return *model;
		}
		else
		{
			return *(loaded[name] = new gl::Model(name));
		}
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
};

}

}