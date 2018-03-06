#pragma once 

#include "object.h"
#include "property.h"
#include "shader.h"
#include "texture.h"
#include <cmath>

namespace bee
{

class WaterMeshBase;

class WaveBase:
	public gl::ShaderController
{
	BEE_SC_INHERIT(WaveBase, gl::ShaderController);
	friend class WaterMeshBase;
};

class WaterMeshBase: public Object
{
	friend class WaveBase;
	static constexpr auto minMeshWidth = .1f;
	static constexpr auto firstStageWidth = 35.f;
	static constexpr auto maxStageCount = 10;
	static constexpr auto stageCount = 10;
protected:
	WaterMeshBase()
		// width(width), length(length)
	{
		static int n = init();
	}
public:
	void render(ViewPort &viewPort) override
	{
		setViewMatrices(viewPort);
		waves.invoke();
		quads[0][0]->render();
		for (int i = 1; i != stageCount; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				quads[i][j]->render();
			}
		}
	}
	// float getWidth() const
	// {
	// 	return width;
	// }
	// float getLength() const
	// {
	// 	return length;
	// }
	void attachWave(WaveBase &wave)
	{
		waves.addController(wave);
	}
protected:
	static int init()
	{
		float meshWidth = minMeshWidth;
		int meshCount = firstStageWidth / meshWidth + 8;
		meshCount -= meshCount % 9;
		float left = -meshCount * meshWidth / 2;
		float top = left;
		auto genVertices = [&](Quad *&quad, int edges)
		{
			int verticesCount, w = (meshCount + 1) << 1, h = meshCount;
			float l = left, t = top;
			switch (edges)
			{
			case 0b0010: case 0b0001: 
				verticesCount = 2 * (meshCount * (meshCount + 1) - 1); break;
			case 0b1010: case 0b0110: case 0b0101: case 0b1001: 
				verticesCount = 2 * (meshCount * (meshCount + 1) - 2); break;
			case 0b1000: case 0b0100: 
				verticesCount = 2 * (meshCount * (meshCount + 1)); break;
			case 0b1111: 
				verticesCount = 2 * (meshCount * (meshCount + 1) - 6); break;
			default: BEE_RAISE(Fatal, "Invalid edge enum");
			}
			if (edges & 0b1000)
			{
				l += meshWidth;
				w -= 2;
			}
			if (edges & 0b0100)
			{
				w -= 2;
			}
			if (edges & 0b0010)
			{
				t += meshWidth;
				h -= 1;
			}
			if (edges & 0b0001)
			{
				h -= 1;
			}
			auto vertices = gl::VertexAttrs<gl::pos2>(verticesCount);
			for (int i = 0; i != h; ++i)
			{
				for (int j = 0; j != w; ++j)
				{
					auto i0 = j & 1 ? i : i + 1, j0 = j >> 1;
					vertices[i * w + j].get<gl::pos2>() = {
						j0 * meshWidth + l, i0 * meshWidth + t
					};
				}
			}
			int cornerBegin = h * w;
			int cornerMask[] = { 0b1010, 0b0110, 0b0101, 0b1001 };
			int cornerDiffX[][8] = {
				{ 0, 0, 1, 0, 0, 1, 1, 0 },
				{ meshCount, 0, -1, 0, 0, -1, -1, 0 },
				{ meshCount, 0, -1, 0, 0, -1, -1, 0 },
				{ 0, 0, 1, 0, 0, 1, 1, 0 }
			};
			int cornerDiffY[][8] = {
				{ 0, 3, 0, -1, -1, 0, 0, -1 },
				{ 0, 3, 0, -1, -1, 0, 0, -1 },
				{ meshCount, -3, 0, 1, 1, 0, 0, 1 },
				{ meshCount, -3, 0, 1, 1, 0, 0, 1 }
			};
			int cornerCount = 0;
			for (int i = 0; i != 4; ++i)
			{
				if ((edges & cornerMask[i]) == cornerMask[i])
				{
					auto x = left, y = top;
					for (int j = 0; j != 8; ++j)
					{
						x += cornerDiffX[i][j] * meshWidth; y += cornerDiffY[i][j] * meshWidth;
						vertices[cornerBegin + cornerCount * 8 + j].get<gl::pos2>() = { x, y };
					}
					cornerCount++;
				}
			}
			int edgeBegin = cornerBegin + cornerCount * 8;
			int edgeMask[] = { 0b0010, 0b0100, 0b0001, 0b1000 };
			int edgeDiffX[][6] = {
				{ 0, 0, 1, 1, 1, 0 },
				{ 0, -1, 0, 0, 0, 1 },
				{ 0, 0, 1, 1, 1, 0 },
				{ 0, 1, 0, 0, 0, -1 }
			};
			int edgeDiffY[][6] = {
				{ 0, 1, 0, 0, 0, -1 },
				{ 0, 0, 1, 1, 1, 0 },
				{ 0, -1, 0, 0, 0, 1 },
				{ 0, 0, 1, 1, 1, 0 }
			};
			float startX[] = { left, left + meshWidth * meshCount, left, left };
			float startY[] = { top, top, top + meshWidth * meshCount, top };
			int edgeCount = 0;
			for (int i = 0; i != 4; ++i)
			{
				if (edges & edgeMask[i])
				{
					float x = startX[i], y = startY[i];
					int n = meshCount / 3;
					if (edgeMask[i] & 0b1100)
					{
						if (edges & 0b0010)
						{
							y += 3 * meshWidth;
							n--;
						}
						if (edges & 0b0001)
						{
							n--;
						}
					}
					if (edgeMask[i] & 0b0011)
					{
						if (edges & 0b1000)
						{
							x += 3 * meshWidth;
							n--;
						}
						if (edges & 0b0100)
						{
							n--;
						}
					}
					for (int j = 0; j != n; ++j)
					{
						for (int k = 0; k != 6; ++k)
						{
							x += edgeDiffX[i][k] * meshWidth; y += edgeDiffY[i][k] * meshWidth;
							vertices[edgeBegin + edgeCount * 6 + k].get<gl::pos2>() = { x, y };
						}
						edgeCount++;
					}
				}
			}
			quad = new Quad(::std::move(vertices));
			quad->stripCount = h;
			quad->stripLength = w;
			quad->cornerBegin = cornerBegin;
			quad->cornerCount = cornerCount;
			quad->edgeBegin = edgeBegin;
			quad->edgeCount = edgeCount;
			// quad->vao.setVertices(vertices);
		};
		genVertices(getQuads()[0][0], 0b1111);
		meshCount /= 3;
		for (int i = 1; i != maxStageCount; ++i)
		{
			meshWidth *= 3;
			auto left0 = left, top0 = top;
			const int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
			const int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
			const int edges[] = { 0b1010, 0b0010, 0b0110, 0b1000, 0b0100, 0b1001, 0b0001, 0b0101 };
			auto blockWidth = meshWidth * meshCount;
			for (int j = 0; j != 8; ++j)
			{
				left = left0 + dx[j] * blockWidth;
				top = top0 + dy[j] * blockWidth;
				genVertices(getQuads()[i][j], edges[j]);
			}
			left = left0 + dx[0] * blockWidth;
			top = top0 + dy[0] * blockWidth;
		}
		return 0;
	}
	struct Quad
	{
		Quad(gl::VertexAttrs<gl::pos2> &&vertices):
			vertices(vertices)
		{
			vao.setVertices(vertices);
		}
		gl::VertexAttrs<gl::pos2> vertices;
		gl::ArrayedVertices vao;
		int stripCount, stripLength;
		int cornerBegin, cornerCount;
		int edgeBegin, edgeCount;

		void render()
		{
			// vao.setVertices(vertices);
			for (int i = 0; i != stripCount; ++i)
			{
				vao.render(GL_TRIANGLE_STRIP, i * stripLength, stripLength);
			}
			for (int i = 0; i != cornerCount; ++i)
			{
				vao.render(GL_TRIANGLE_FAN, cornerBegin + i * 8, 8);
			}
			for (int i = 0; i != edgeCount; ++i)
			{
				vao.render(GL_TRIANGLE_FAN, edgeBegin + i * 6, 6);
			}
		}
	};
	using refQuads = Quad *(&)[maxStageCount][8];
	static refQuads getQuads()
	{
		static Quad *quads[maxStageCount][8];
		return quads;
	}
protected:
	refQuads quads = getQuads();
protected:
	// float width = 0, length = 1;
	gl::ShaderControllers waves;
};

class OceanMesh: public WaterMeshBase
{
public:
	void render(ViewPort &viewPort) override
	{
		shader->use();
		material.use();
		WaterMeshBase::render(viewPort);
	}
protected:
	static gl::Shader *getShader()
	{
		static auto var = new gl::Shader(
			gl::VertexShader("ocean-vs.glsl"),
			gl::FragmentShader("ocean-fs.glsl")
		);
		return var;
	}
private:
	gl::Shader *shader = getShader();
	gl::Material material = gl::Material("ocean");
};

}