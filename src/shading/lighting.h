#pragma once

#include "shaderController.h"
#include "property.h"

namespace bee
{

class LightBase:
	public ShaderController
{
	BEE_SC_BASE(LightBase);
public:
	void invoke() override
	{
		gColor = fColor;
		gAmbientIntensity = fAmbientIntensity;
		gDiffuseIntensity = fDiffuseIntensity;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3, Color);
	BEE_SC_UNIFORM(float, AmbientIntensity);
	BEE_SC_UNIFORM(float, DiffuseIntensity);
public:
	BEE_PROPERTY(glm::vec3, Color) = { 1, 1, 1 };
	BEE_PROPERTY(float, AmbientIntensity) = 1.f;
	BEE_PROPERTY(float, DiffuseIntensity) = 1.f;
};

class LightBaseMulti:
	public ShaderController
{
	BEE_SC_BASE_MULTI(LightBaseMulti);
public:
	void invoke() override
	{
		gColor[getIndex()] = fColor;
		gAmbientIntensity[getIndex()] = fAmbientIntensity;
		gDiffuseIntensity[getIndex()] = fDiffuseIntensity;
	}
protected:
	virtual int getIndex() const = 0;
protected:
	BEE_SC_UNIFORM(::glm::vec3[], Color);
	BEE_SC_UNIFORM(float[], AmbientIntensity);
	BEE_SC_UNIFORM(float[], DiffuseIntensity);
public:
	BEE_PROPERTY(glm::vec3, Color) = { 1, 1, 1 };
	BEE_PROPERTY(float, AmbientIntensity) = 1.f;
	BEE_PROPERTY(float, DiffuseIntensity) = 1.f;
};

class DirectionalLight:
	public LightBase
{
	BEE_SC_INHERIT(DirectionalLight, LightBase);
public:
	DirectionalLight(const ::glm::vec3 &direction):
		BEE_SC_SUPER(), fDirection(direction)
	{
	}

	void invoke() override
	{
		Super::invoke();
		gDirection = fDirection;
	}
	BEE_SC_UNIFORM(::glm::vec3, Direction);
public:
	BEE_PROPERTY(::glm::vec3, Direction) = { 0, 0, -1 };
};

class PointLight:
	public LightBaseMulti
{
	BEE_SC_INHERIT_MULTI(PointLight, LightBaseMulti);
public:
	PointLight(const ::glm::vec3 &position):
		BEE_SC_SUPER(), fPosition(position)
	{
	}

	void invoke() override
	{
		Super::invoke();
		gPosition[getIndex()] = fPosition;

		gAttenConstant[getIndex()] = fAttenConstant;
		gAttenLinear[getIndex()] = fAttenLinear;
		gAttenExp[getIndex()] = fAttenExp;
	}
protected:
	int getIndex() const override
	{
		return 0;
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
	BEE_SC_INHERIT_MULTI(SpotLight, PointLight);
public:
	SpotLight(const ::glm::vec3 &position):
		BEE_SC_SUPER()
	{
		setPosition(position);
	}

	void invoke() override
	{
		Super::invoke();
		gDirection[getIndex()] = fDirection;
		gCutoff[getIndex()] = fCutoff;
	}
protected:
	int getIndex() const override
	{
		return 0;
	}
protected:
	BEE_SC_UNIFORM(::glm::vec3[], Direction);
	BEE_SC_UNIFORM(float[], Cutoff);
public:
	BEE_PROPERTY(::glm::vec3, Direction) = { 0, 0, 1 };
	BEE_PROPERTY(float, Cutoff) = 1.f;
};

}