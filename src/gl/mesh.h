#pragma once

#include "common.h"
#include "buffers.h"
#include "vertexAttr.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace bee
{

namespace gl
{

class MeshBase
{
public:
	MeshBase(): 
		vao(nullptr) 
	{
	}
	MeshBase(const MeshBase &other):
		vao(new VAO(*other.vao))
	{
	}
	MeshBase(MeshBase &&other):
		vao(other.vao)
	{
		other.vao = nullptr;
	}
	MeshBase &operator = (const MeshBase &other)
	{
		delete vao;
		vao = new VAO(*other.vao);
		return *this;
	}
	MeshBase &operator = (MeshBase &&other)
	{
		vao = other.vao;
		other.vao = nullptr;
		return *this;
	}
	virtual ~MeshBase()
	{
		delete vao;
	}

	virtual void render() const
	{
		vao->render();
	}
protected:
	template <typename ...Attrs>
	void initVAO(const VertexAttrs<Attrs...> &vertices, const Faces &faces)
	{
		delete vao;
		vao = new VAO(vertices, faces);
	}
private:
	VAO *vao;
};

class StaticMesh: public MeshBase
{
public:
	template <typename ...Attrs>
	StaticMesh(const VertexAttrs<Attrs...> &vertices, const Faces &faces)
	{
		MeshBase::initVAO(vertices, faces);
	}
};

class Mesh: private MeshBase
{
public:
	Mesh(aiMesh *mesh)
	{
		VertexAttrs<any> vertices(mesh->mNumVertices);
		bool pos = vertices.template invoke<pos3>(mesh->HasPositions());
		// bool color = vertices.template invoke<color4>(mesh->HasVertexColors());
		bool norm = vertices.template invoke<norm3>(mesh->HasNormals());
		bool tg = vertices.template invoke<tg3>(mesh->HasTangentsAndBitangents());
		bool bitg = vertices.template invoke<bitg3>(mesh->HasTangentsAndBitangents());
		// bool tex = vertices.template invoke<tex3>(mesh->HasTextureCoords());
		vertices.alloc();
		if (pos)
		{
			auto fp = mesh->mVertices;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<pos3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		// if (color)
		// {
		// 	auto fp = mesh->mColors;
		// 	for (auto i = 0u; i != mesh->mNumVertices; ++i)
		// 	{
		// 		auto &vec = vertices.template get<color4>(i);
		// 		vec = *reinterpret_cast<decltype(vec)*>(fp++);
		// 	}
		// }
		if (norm)
		{
			auto fp = mesh->mNormals;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<norm3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (tg)
		{
			auto fp = mesh->mTangents;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<tg3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (bitg)
		{
			auto fp = mesh->mBitangents;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<bitg3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		// if (tex)
		// {
		// 	auto fp = mesh->mTextureCoords;
		// 	for (auto i = 0u; i != mesh->mNumVertices; ++i)
		// 	{
		// 		auto &vec = vertices.template get<tex3>(i);
		// 		vec = *reinterpret_cast<decltype(vec)*>(fp++);
		// 	}
		// }
		Faces faces(mesh->mNumFaces);
		auto fp = mesh->mFaces;
		for (auto &vec: faces)
		{
			vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++->mIndices);
		}
		initVAO(vertices, faces);
	}
	void render() const override
	{
		MeshBase::render();
	}
};

}

}