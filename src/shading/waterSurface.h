#pragma once 

#include "gl.h"
#include "object.h"
#include "property.h"
#include <cmath>

namespace bee
{

class WaterSurface;

class WaveBase
{
	friend class WaterSurface;
protected:
	WaveBase() = default;
protected:
	virtual void invoke(::glm::vec3[][200], int count, int length) = 0;
};

class WaterSurface: public Object
{
	friend class WaveBase;
	static constexpr auto meshWidth = .1f;
public:
	WaterSurface() = default;

	void render(ViewPort &viewPort) override
	{
		Object::render(viewPort);
		for (int i = 0; i != stripCount; ++i)
		{
			for (int j = 0; j != stripLength; ++j)
			{
				strip[i][j].z = 0.f;
			}
		}
		for (auto &wave: waves)
		{
			wave->invoke(strip, stripCount, stripLength);
		}
		genNormals();
		int len = stripLength << 1;
		for (int i = 0; i != stripCount - 1; ++i)
		{
			for (int j = 0; j != len; ++j)
			{
				auto i0 = j & 1 ? i : i + 1;
				auto j0 = j >> 1;
				vertices[i * len + j].get<gl::pos3>() = strip[i0][j0];
				vertices[i * len + j].get<gl::norm3>() = strip[i0][j0];
			}
		}
		vbo.bind(); 
		vbo.data(vertices.size() * vertices.elemSize, vertices.begin());
		vertices.setVertexAttribute();
		for (int i = 0; i != stripCount - 1; ++i)
		{
			glDrawArrays(GL_TRIANGLE_STRIP, i * len, len);
		}
		// for (int i = 0; i != (stripCount - 1) * len; ++i)
		// {
		// 	// for (int j = 0; j != stripLength; ++j)
		// 	// {
		// 	std::cerr << "(" << vertices[i].get<gl::pos3>().x << "," <<
		// 		vertices[i].get<gl::pos3>().y << "," <<
		// 		vertices[i].get<gl::pos3>().z << ") ";
		// 	// }
		// 	std::cerr << std::endl;
		// }
	}
	void resize(int count, int length)
	{
		stripCount = count;
		stripLength = length;
		for (int i = 0; i != stripCount; ++i)
		{
			for (int j = 0; j != stripLength; ++j)
			{
				strip[i][j].x = j * meshWidth;
				strip[i][j].y = i * meshWidth;
			}
		}
		vertices.resize( (stripCount - 1) * (stripLength << 1) );
	}
	int getStripCount() const
	{
		return stripCount;
	}
	int getStripLength() const
	{
		return stripLength;
	}
	void attachWave(WaveBase *wave)
	{
		waves.push_back(wave);
	}
private:
	void genNormals()
	{
		for (int i = 1; i < stripCount - 1; ++i)
		{
			for (int j = 1; j < stripLength - 1; ++j)
			{
				auto p0 = strip[i][j - 1] - strip[i][j];
				auto p1 = strip[i - 1][j] - strip[i][j];
				auto p2 = strip[i][j + 1] - strip[i][j];
				auto p3 = strip[i + 1][j] - strip[i][j];
				normals[i][j] = ::glm::cross(p1, p0);
				normals[i][j] += ::glm::cross(p2, p1);
				normals[i][j] += ::glm::cross(p3, p2);
				normals[i][j] += ::glm::cross(p0, p3);
			}
		}
		for (int j = 1; j < stripLength - 1; ++j)
		{
			auto p0 = strip[0][j - 1] - strip[0][j];
			// auto p1 = strip[i - 1][j] - strip[i][j];
			auto p2 = strip[0][j + 1] - strip[0][j];
			auto p3 = strip[0 + 1][j] - strip[0][j];
			// normals[stripCount][j] += ::glm::cross(p1, p0);
			// normals[stripCount][j] += ::glm::cross(p2, p1);
			normals[0][j] = ::glm::cross(p3, p2);
			normals[0][j] += ::glm::cross(p0, p3);
		}
		for (int j = 1; j < stripLength - 1; ++j)
		{
			auto p0 = strip[stripCount - 1][j - 1] - strip[stripCount - 1][j];
			auto p1 = strip[stripCount - 1 - 1][j] - strip[stripCount - 1][j];
			auto p2 = strip[stripCount - 1][j + 1] - strip[stripCount - 1][j];
			// auto p3 = strip[i + 1][j] - strip[i][j];
			normals[stripCount - 1][j] = ::glm::cross(p1, p0);
			normals[stripCount - 1][j] += ::glm::cross(p2, p1);
			// normals[i][j] += ::glm::cross(p3, p2);
			// normals[i][j] += ::glm::cross(p0, p3);
		}
		for (int i = 1; i < stripCount - 1; ++i)
		{
			// auto p0 = strip[i][j - 1] - strip[i][j];
			auto p1 = strip[i - 1][0] - strip[i][0];
			auto p2 = strip[i][0 + 1] - strip[i][0];
			auto p3 = strip[i + 1][0] - strip[i][0];
			// normals[i][0] += ::glm::cross(p1, p0);
			normals[i][0] = ::glm::cross(p2, p1);
			normals[i][0] += ::glm::cross(p3, p2);
			// normals[i][0] += ::glm::cross(p0, p3);
		}
		for (int i = 1; i < stripCount - 1; ++i)
		{
			auto p0 = strip[i][stripLength - 1 - 1] - strip[i][stripLength - 1];
			auto p1 = strip[i - 1][stripLength - 1] - strip[i][stripLength - 1];
			// auto p2 = strip[i][j + 1] - strip[i][j];
			auto p3 = strip[i + 1][stripLength - 1] - strip[i][stripLength - 1];
			normals[i][stripLength - 1] = ::glm::cross(p1, p0);
			// normals[i][j] += ::glm::cross(p2, p1);
			// normals[i][j] += ::glm::cross(p3, p2);
			normals[i][stripLength - 1] += ::glm::cross(p0, p3);
		}
		normals[0][0] = 
			::glm::cross(strip[1][0] - strip[0][0], 
				strip[0][1] - strip[0][0]);
		normals[0][stripLength - 1] = 
			::glm::cross(strip[0][stripLength - 1 - 1] - strip[0][stripLength - 1], 
				strip[1][stripLength - 1] - strip[0][stripLength - 1]);
		normals[stripCount - 1][stripLength - 1] = 
			::glm::cross(strip[stripCount - 1 - 1][stripLength - 1] - strip[stripCount - 1][stripLength - 1], 
				strip[stripCount - 1][stripLength - 1 - 1] - strip[stripLength - 1][stripLength - 1]);
		normals[stripCount - 1][0] = 
			::glm::cross(strip[stripCount - 1][1] - strip[stripCount - 1][0], 
				strip[stripCount - 1 - 1][0] - strip[stripCount - 1][0]);
		for (int i = 0; i != stripCount; ++i)
		{
			for (int j = 0; j != stripLength; ++j)
			{
				normals[i][j] = ::glm::normalize(normals[i][j]);
			}
		}
	}
protected:
	::std::vector<WaveBase*> waves;
	gl::VBO vbo;
	gl::VertexAttrs<gl::pos3, gl::norm3> vertices = 
		gl::VertexAttrs<gl::pos3, gl::norm3>(1);
	int stripCount, stripLength;
	::glm::vec3 strip[200][200], normals[200][200];
};

class GerstnerWave: public WaveBase
{
	static constexpr int sampleCount = 12;
public:
	GerstnerWave()
	{
		setSample();
	}
protected:
	void invoke(::glm::vec3 strip[][200], int count, int length) override
	{
		for (int i = 0; i != count; ++i)
		{
			for (int j = 0; j != length; ++j)
			{
				auto d = (strip[i][j].x/*- x0*/) * cosf(fAngle) + 
					(strip[i][j].y/*- y0*/) * sinf(fAngle);
				auto z = gerstnerZ(fLength, fHeight, d + fSpeed * time);
				strip[i][j].z += z;
			}
		}
	}
	static constexpr float time = 0;
protected:
	float gerstnerZ(float w, float h, float x) const
	{
		x = fmodf(x * 2 * sampleX[sampleCount - 1] / w, 2 * sampleX[sampleCount - 1]);
		if (x > sampleX[sampleCount - 1])
		{
			x = 2 * sampleX[sampleCount - 1] - x;
		}
		// search for xpos section
		int l = 0, r = sampleCount, m = (l + r) >> 1;
		while (r - l > 1)
		{
			if (sampleX[m] > x)
			{
				r = m;
			}
			else
			{
				l = m;
			}
			m = (l + r) >> 1;
		}
		// linear interpolation
		return h * (sampleY[l] + (x - sampleX[l]) 
			/ (sampleX[r] - sampleX[l]) * (sampleY[r] - sampleY[l]));
	}
	void setSample()
	{
		// float start = M_PI / sampleCount;
		// float det = M_PI / sampleCount / (sampleCount + 1);
		float det = 4 * M_PI * fSharpness / (3.f * (sampleCount - 1) * (sampleCount - 2));
		float start = M_PI * (1 - 2.f / 3.f * fSharpness) / (sampleCount - 1) + (sampleCount - 1) * det;
		auto alpha = 0.f;
		for (int i = 0; i != sampleCount; ++i)
		{
			sampleX[i] = alpha - fSharpness * fLength * sinf(alpha - M_PI);
			sampleY[i] = cosf(alpha - M_PI);
			BEE_LOG(alpha, " ", sampleX[i], " ", sampleY[i]);
			alpha += start; start -= det;
		}
	}
protected:
	float sampleX[sampleCount];
	float sampleY[sampleCount];
public:
	BEE_PROPERTY(float, Sharpness) = .5f;
	BEE_PROPERTY(float, Length) = 2.f;
	BEE_PROPERTY(float, Height) = .2f;
	BEE_PROPERTY(float, Angle) = 0.f;
	BEE_PROPERTY(float, Speed) = .1f;
};

}