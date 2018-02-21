#pragma once 

#include "object.h"
#include "property.h"
#include "shader.h"
#include "texture.h"
#include <cmath>

namespace bee
{

class WaterSurface;

class WaveBase:
	public gl::ShaderControllerMulti
{
	BEE_SC_INHERIT(WaveBase, gl::ShaderControllerMulti);
	friend class WaterSurface;
};

class WaterSurface: public Object
{
	friend class WaveBase;
	static constexpr auto meshWidth = .1f;
public:
	WaterSurface()
	{
		resize(width, length);
	}
	WaterSurface(float width, float length):
		width(width), length(length)
	{
		resize(width, length);
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		setViewMatrices(viewPort);
		// BEE_LOG()
		waves.invoke();
		// texture.invoke(0);
		int len = stripLength << 1;
		for (int i = 0; i != stripCount - 1; ++i)
		{
			vao.render(GL_TRIANGLE_STRIP, i * len, len);
		}
	}
	void resize(float x, float y)
	{
		width = x; length = y;
		stripCount = y / meshWidth;
		stripLength = x / meshWidth;
		auto vertices = gl::VertexAttrs<gl::pos3>(
				(stripCount - 1) * (stripLength << 1));
		int len = stripLength << 1;
		float dx = width / 2, dy = length / 2;
		for (int i = 0; i != stripCount - 1; ++i)
		{
			for (int j = 0; j != len; ++j)
			{
				auto i0 = j & 1 ? i : i + 1;
				auto j0 = j >> 1;
				vertices[i * len + j].get<gl::pos3>() = {
					j0 * meshWidth - dx, i0 * meshWidth - dy, 0.f
				};
			}
		}
		vao.setVertices(vertices);
	}
	float getWidth() const
	{
		return width;
	}
	float getLength() const
	{
		return length;
	}
	void attachWave(WaveBase &wave)
	{
		waves.addController(wave);
	}
private:
	static gl::Shader *getShader()
	{
		static auto var = new gl::Shader(
			gl::VertexShader("waterSurface-vs.glsl"),
			gl::FragmentShader("waterSurface-fs.glsl")
		);
		return var;
	}
private:
	gl::Shader *shader = getShader();
protected:
	gl::ArrayedVAO vao;
	int stripCount, stripLength;
	float width = 0, length = 1;
	// gl::Texture<gl::Tex2D> texture = 
		// gl::Texture<gl::Tex2D>("water-texture-2.tga");
	gl::ShaderControllers waves;
};

class GerstnerWave: 
	public WaveBase
{
	BEE_SC_INHERIT(GerstnerWave, WaveBase);
public:
	GerstnerWave():
		BEE_SC_SUPER()
	{
	}
protected:
	bool invoke(int index) override
	{
		gSteepness[index] = fSteepness / (fFrequency * fAmplitude);
		gAmplitude[index] = fAmplitude;
		gFrequency[index] = fFrequency;
		gSpeed[index] = fSpeed;
		gDirection[index] = ::glm::normalize(fDirection);
		return true;
	}
protected:
	BEE_SC_UNIFORM(float[], Steepness);
	BEE_SC_UNIFORM(float[], Amplitude);
	BEE_SC_UNIFORM(float[], Frequency);
	BEE_SC_UNIFORM(float[], Speed);
	BEE_SC_UNIFORM(::glm::vec2[], Direction);
public:
	BEE_PROPERTY(float, Steepness) = .5f;
	BEE_PROPERTY(float, Amplitude) = .2f;
	BEE_PROPERTY(float, Frequency) = 5.f;
	BEE_PROPERTY(::glm::vec2, Direction) = {1, 0};
	BEE_PROPERTY(float, Speed) = 1.f;
};

}