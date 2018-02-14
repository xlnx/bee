#pragma once

#include "shaderController.h"

namespace bee
{

class LightBase
{
public:
	::glm::vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

class DirectionalLight: 
	public ShaderController, 
	protected LightBase
{
protected:
	::glm::vec3 direction;

	gl::UniformRef<::glm::vec3> gColor;
	gl::UniformRef<float> gAmbientIntensity;
	gl::UniformRef<float> gDiffuseIntensity;
	gl::UniformRef<::glm::vec3> gDirection;
public:
	DirectionalLight(const ::glm::vec3 &direction, 
			const ::glm::vec3 &color = {1, 1, 1}, 
			float ambientIntensity = 1, 
			float diffuseIntensity = 1):
		LightBase{color, ambientIntensity, diffuseIntensity}, 
		direction(direction),
		gColor(gl::Shader::uniform<::glm::vec3>("directionalLight.color")),
		gAmbientIntensity(gl::Shader::uniform<float>("directionalLight.ambientIntensity")),
		gDiffuseIntensity(gl::Shader::uniform<float>("directionalLight.diffuseIntensity")),
		gDirection(gl::Shader::uniform<::glm::vec3>("directionalLight.direction"))
	{
	}

	void invoke() override
	{
		gColor = color;
		gAmbientIntensity = ambientIntensity;
		gDiffuseIntensity = diffuseIntensity;
		gDirection = direction;
	}
};

}