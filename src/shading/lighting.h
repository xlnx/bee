#pragma once

#include "shader.h"
#include "property.h"

namespace bee
{

class LightBaseSingle:
	public gl::ShaderControllerSingle
{
	BEE_SC_INHERIT(LightBaseSingle, gl::ShaderControllerSingle);
public:
	bool invoke(int index) override
	{
		gColor = fColor;
		gAmbientIntensity = fAmbientIntensity;
		gDiffuseIntensity = fDiffuseIntensity;
		return true;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3, Color);
	BEE_SC_UNIFORM(float, AmbientIntensity);
	BEE_SC_UNIFORM(float, DiffuseIntensity);
public:
	BEE_PROPERTY(glm::vec3, Color) = { 1, 1, 1 };
	BEE_PROPERTY(float, AmbientIntensity) = .5f;
	BEE_PROPERTY(float, DiffuseIntensity) = 1.f;
};

class LightBaseMulti:
	public gl::ShaderControllerMulti
{
	BEE_SC_INHERIT(LightBaseMulti, gl::ShaderControllerMulti);
public:
	bool invoke(int index) override
	{
		gColor[index] = fColor;
		gAmbientIntensity[index] = fAmbientIntensity;
		gDiffuseIntensity[index] = fDiffuseIntensity;
		return true;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3[], Color);
	BEE_SC_UNIFORM(float[], AmbientIntensity);
	BEE_SC_UNIFORM(float[], DiffuseIntensity);
public:
	BEE_PROPERTY(glm::vec3, Color) = { 1, 1, 1 };
	BEE_PROPERTY(float, AmbientIntensity) = .5f;
	BEE_PROPERTY(float, DiffuseIntensity) = 1.f;
};

class DirectionalLight:
	public LightBaseSingle
{
	BEE_SC_INHERIT(DirectionalLight, LightBaseSingle);
public:
	DirectionalLight(const ::glm::vec3 &direction):
		BEE_SC_SUPER(), fDirection(direction)
	{
	}

	bool invoke(int index) override
	{
		Super::invoke(index);
		gDirection = fDirection;
		return true;
	}
	BEE_SC_UNIFORM(::glm::vec3, Direction);
public:
	BEE_PROPERTY(::glm::vec3, Direction) = { 0, 0, -1 };
};

class PointLight:
	public LightBaseMulti
{
	BEE_SC_INHERIT(PointLight, LightBaseMulti);
public:
	PointLight(const ::glm::vec3 &position):
		BEE_SC_SUPER(), fPosition(position)
	{
	}

	bool invoke(int index) override
	{
		Super::invoke(index);
		gPosition[index] = fPosition;

		gAttenConstant[index] = fAttenConstant;
		gAttenLinear[index] = fAttenLinear;
		gAttenExp[index] = fAttenExp;
		return true;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3[], Position);

	BEE_SC_UNIFORM(float[], AttenConstant);
	BEE_SC_UNIFORM(float[], AttenLinear);
	BEE_SC_UNIFORM(float[], AttenExp);
public:
	BEE_PROPERTY(::glm::vec3, Position) = { 0, 0, 1 };
	BEE_PROPERTY(float, AttenConstant) = 1.f;
	BEE_PROPERTY(float, AttenLinear) = .5f;
	BEE_PROPERTY(float, AttenExp) = .5f;
};

class SpotLight:
	public PointLight
{
	BEE_SC_INHERIT(SpotLight, PointLight);
public:
	SpotLight(const ::glm::vec3 &position):
		BEE_SC_SUPER()
	{
		setPosition(position);
	}

	bool invoke(int index) override
	{
		Super::invoke(index);
		gDirection[index] = fDirection;
		gCutoff[index] = fCutoff;
		return true;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3[], Direction);
	BEE_SC_UNIFORM(float[], Cutoff);
public:
	BEE_PROPERTY(::glm::vec3, Direction) = { 0, 0, 1 };
	BEE_PROPERTY(float, Cutoff) = 1.f;
};

}