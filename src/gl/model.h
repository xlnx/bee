#pragma once

#include "common.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

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

class Model: private ModelBase
{
public:
	Model() = default;
	Model(const ::std::string &file)
	{
		Assimp::Importer ass;
		scene = ass.ReadFile(file, 
			aiProcess_Triangulate | 
			aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			BEE_RAISE(GLFatal, "Failed to import model(" + file + "). Assimp: " + ass.GetErrorString());
		}
		auto root = scene->mRootNode;
		for (auto i = 0u; i != root->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[root->mMeshes[i]];
			meshes.emplace_back(mesh);
		}
		// BEE_LOG(root->mNumChildren);
		// for (auto i = 0u; i != root->mNumChildren; ++i)
		// {

		// }
	}

	void render() const override
	{
		// BEE_LOG(meshes.size());
		for (auto &mesh: meshes) mesh.render();
	}
protected:
	::std::vector<Mesh> meshes;
	const aiScene *scene = nullptr;
};

}

}