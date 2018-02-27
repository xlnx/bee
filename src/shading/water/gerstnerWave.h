#pragma once

#include "waterMesh.h"

namespace bee
{

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