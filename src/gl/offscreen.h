#pragma once

#include "buffers.h"
#include "texture.h"
#include "viewPort.h"
#include "windowBase.h"

namespace bee
{

namespace gl
{

class CubeDepthFBT: public CubeDepthTexture
{
public:
	CubeDepthFBT():
		CubeDepthTexture(WindowBase::getWidth(), WindowBase::getHeight())
	{
		fbo.bind();
			texture2D.attachTo(fbo);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			fbo.validate();
		fbo.unbind();
	}
	void bind(int i)
	{
		fbo.bind();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *this, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		cubeface = i;
	}
	void unbind()
	{
		fbo.unbind();
	}
	void configureCamera(ViewPort &viewPort)
	{
		static const ::glm::vec3 cameraInfos[][2] = {
			{ {1.f, 0.f, 0.f},  {0.f, -1.f, 0.f} },
			{ {-1.f, 0.f, 0.f}, {0.f, -1.f, 0.f} },
			{ {0.f, 1.f, 0.f},  {0.f, 0.f, 1.f} },
			{ {0.f, -1.f, 0.f}, {0.f, 0.f, -1.f} },
			{ {0.f, 0.f, 1.f},  {0.f, -1.f, 0.f} },
			{ {0.f, 0.f, -1.f}, {0.f, -1.f, 0.f} }
		};
		viewPort.setTarget(cameraInfos[cubeface][0]);
		viewPort.setUp(cameraInfos[cubeface][1]);
	}
private:
	FBO fbo;
	DepthTexture texture2D = DepthTexture(WindowBase::getWidth(), WindowBase::getHeight());
	int cubeface = 0;
};

class SingleChannelFBT: public SingleChannelTexture
{
public:
	SingleChannelFBT():
		SingleChannelTexture(WindowBase::getWidth(), WindowBase::getHeight())
	{
		fbo.bind();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_2D, *this, 0);
		fbo.unbind();
	}

	void bind()
	{
		fbo.bind();
	}
	void unbind()
	{
		fbo.unbind();
	}
private:
	FBO fbo;
};

}

}