#pragma once

#include "common.h"
#include "buffers.h"
#include "vertexAttr.h"
#include "material.h"
#include "shader.h"
#include "property.h"
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
		delete vao;
		vao = other.vao;
		other.vao = nullptr;
		return *this;
	}
	virtual ~MeshBase()
	{
		delete vao;
	}

	void render() const
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

class Mesh: private MeshBase
{
	BEE_UNIFORM(int, Diffuse, "Material");
	BEE_UNIFORM(int, Specular, "Material");
	BEE_UNIFORM(int, Ambient, "Material");
	BEE_UNIFORM(int, Emissive, "Material");
	BEE_UNIFORM(int, Normals, "Material");
public:
	template <typename ...Attrs>
	Mesh(const VertexAttrs<Attrs...> &vertices, const Faces &faces)
	{
		MeshBase::initVAO(vertices, faces);
	}
	Mesh(const Material *material, aiMesh *mesh): 
		material(material)
	{
		VertexAttrs<any> vertices(mesh->mNumVertices);
		vertices.template invoke<pos3>(mesh->HasPositions());
		vertices.template invoke<color4>(mesh->HasVertexColors(0));
		vertices.template invoke<norm3>(mesh->HasNormals());
		vertices.template invoke<tg3>(mesh->HasTangentsAndBitangents());
		vertices.template invoke<bitg3>(mesh->HasTangentsAndBitangents());
		vertices.template invoke<tex3>(mesh->HasTextureCoords(0));
		vertices.alloc();
		BEE_LOG("read ", mesh->mVertices, " vertices");
		if (mesh->HasPositions())
		{
			BEE_LOG("has positions");
			auto fp = mesh->mVertices;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<pos3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasVertexColors(0))
		{
			BEE_LOG("has colors");
			auto fp = mesh->mColors[0];
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<color4>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasNormals())
		{
			BEE_LOG("has normals");
			auto fp = mesh->mNormals;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<norm3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasTangentsAndBitangents())
		{
			BEE_LOG("has tangent");
			auto fp = mesh->mTangents;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<tg3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasTangentsAndBitangents())
		{
			BEE_LOG("has bitangent");
			auto fp = mesh->mBitangents;
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<bitg3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasTextureCoords(0))
		{
			BEE_LOG("has texcoords");
			auto fp = mesh->mTextureCoords[0];
			for (auto i = 0u; i != mesh->mNumVertices; ++i)
			{
				auto &vec = vertices.template get<tex3>(i);
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++);
			}
		}
		if (mesh->HasFaces())
		{
			Faces faces(mesh->mNumFaces);
			auto fp = mesh->mFaces;
			for (auto &vec: faces)
			{
				vec = *reinterpret_cast<typename ::std::remove_reference<decltype(vec)>::type*>(fp++->mIndices);
			}
			initVAO(vertices, faces);
		}
		else
		{
			BEE_RAISE(GLFatal, "The mesh object has no faces info.");
		}
	}
	void render() const
	{
		if (material)
		{
			if (auto texture = material->getTexture<Diffuse>())
			{
				if (gDiffuse = Diffuse)
				{
					glActiveTexture(GL_TEXTURE0 + Diffuse);
					glBindTexture(Tex2D, texture);
				}
			}
			if (auto texture = material->getTexture<Specular>())
			{
				if (gSpecular = Specular)
				{
					glActiveTexture(GL_TEXTURE0 + Specular);
					glBindTexture(Tex2D, texture);
				}
			}
			if (auto texture = material->getTexture<Ambient>())
			{
				if (gAmbient = Ambient)
				{
					glActiveTexture(GL_TEXTURE0 + Ambient);
					glBindTexture(Tex2D, texture);
				}
			}
			if (auto texture = material->getTexture<Emissive>())
			{
				if (gEmissive = Emissive)
				{
					glActiveTexture(GL_TEXTURE0 + Emissive);
					glBindTexture(Tex2D, texture);
				}
			}
			if (auto texture = material->getTexture<Normals>())
			{
				if (gNormals = Normals)
				{
					glActiveTexture(GL_TEXTURE0 + Normals);
					glBindTexture(Tex2D, texture);
				}
			}
		}
		MeshBase::render();
	}
private:
	const Material *material = nullptr;
};

}

}