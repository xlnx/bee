#pragma once 

#include "gl.h"
#include "object.h"
#include "property.h"
#include "shaderController.h"
#include "texture.h"
#include <cmath>

namespace bee
{

class WaterSurface;

class WaveBase:
	public ShaderControllerMulti
{
	BEE_SC_INHERIT(WaveBase, ShaderControllerMulti);
	friend class WaterSurface;
};

class WaterSurface: public Object
{
	friend class WaveBase;
	static constexpr auto meshWidth = .02f;
public:
	WaterSurface(): Object(* new gl::Shader(
			gl::VertexShader("testWave.vert"),
			gl::FragmentShader("testWave.frag")
		))
	{
	}

	void render(ViewPort &viewPort) override
	{
		Object::render(viewPort);
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
		stripCount = y / meshWidth;
		stripLength = x / meshWidth;
		auto vertices = gl::VertexAttrs<gl::pos3>(
				(stripCount - 1) * (stripLength << 1));
		int len = stripLength << 1;
		for (int i = 0; i != stripCount - 1; ++i)
		{
			for (int j = 0; j != len; ++j)
			{
				auto i0 = j & 1 ? i : i + 1;
				auto j0 = j >> 1;
				vertices[i * len + j].get<gl::pos3>() = {
					j0 * meshWidth, i0 * meshWidth, 0.f
				};
			}
		}
		vao.setVertices(vertices);
	}
	int getStripCount() const
	{
		return stripCount;
	}
	int getStripLength() const
	{
		return stripLength;
	}
	void attachWave(WaveBase &wave)
	{
		waves.addController(wave);
	}
protected:
	gl::ArrayedVAO vao;
	int stripCount, stripLength;
	gl::Texture<gl::Tex2D> texture = 
		gl::Texture<gl::Tex2D>("water-texture-2.tga");
	
	ShaderControllers<WaveBase> waves;
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
		gDirection[index] = fDirection;
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