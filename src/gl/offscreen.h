#pragma once

#include "buffers.h"
#include "texture.h"
#include "viewPort.h"
#include "windowBase.h"

namespace bee
{

namespace gl
{

class CubeDepthFBT: public CubeDepthTexture//, private FBO
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
	// using fbo.unbind;
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

class SingleChannelFBT: public SingleChannelTexture//, private FBO
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

	// using fbo.bind, fbo.unbind;
	void bind() const
	{
		fbo.bind();
	}
	void unbind() const
	{
		fbo.unbind();
	}
private:
	FBO fbo;
};

class SingleChannelFBRB //, private RBO, private FBO
{
public:
	SingleChannelFBRB()
	{
		fbo.bind();
			rbo.bind();
				glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, 
					WindowBase::getWidth(), WindowBase::getHeight());
				fbo.addRenderBuffer<GL_COLOR_ATTACHMENT0>(rbo);
			rbo.unbind();
			dbo.bind();
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 
					WindowBase::getWidth(), WindowBase::getHeight());
				fbo.addRenderBuffer<GL_DEPTH_ATTACHMENT>(dbo);
			dbo.unbind();
		fbo.unbind();
	}

	// using fbo.bind, fbo.unbind;
	void bind() const
	{
		fbo.bind(); //rbo.bind();
	}
	void unbind() const
	{
		fbo.unbind(); //rbo.unbind(); 
	}
private:
	RBO rbo, dbo; FBO fbo;
};

}

}