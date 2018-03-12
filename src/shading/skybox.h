#pragma once 

#include "object.h"
#include "viewPort.h"
#include "shader.h"
#include "texture.h"
#include "modelManager.h"

namespace bee
{

class Skybox: public Object
{
	BEE_UNIFORM_GLOB(int, CubemapTexture);
public:
	Skybox()
	{
		scale(1e5, 1e5, 1e5);
	}

	void render(ViewPort &viewPort) override
	{
		shader->use();
		GLint oldCullFaceMode, oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		setViewMatrices(viewPort);
		cubeTexture.invoke(1);
		gCubemapTexture = 1;
		skybox.render();
		shader->unuse();

		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
	}
private:
	static gl::Shader *getShader()
	{
		static gl::Shader *shader = new gl::Shader(
			gl::VertexShader("skybox-vs.glsl"),
			gl::FragmentShader("skybox-fs.glsl")
		);
		return shader;
	}
private:
	gl::Model skybox = ModelManager::loadModel("skybox/skybox.obj");
	gl::Texture3D cubeTexture = gl::Texture3D("textures/skybox/bluesky.jpg");
	gl::Shader *shader = getShader();
};

}