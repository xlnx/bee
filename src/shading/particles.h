#pragma once 

#include "gl.h"
#include "object.h"
#include "property.h"
// #include "shaderController.h"

namespace bee
{

struct ParticleBase
{
	virtual void setPosition(const ::glm::vec3 &position) = 0;
};

struct FlameParticle: ParticleBase
{
	float type;
	::glm::vec3 position;
	::glm::vec3 velocity;
	float lifeTimeMillis;
	void setPosition(const ::glm::vec3 &position) override
	{
		this->position = position;
	}
	static void enable()
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 1, GL_FLOAT, false, sizeof(FlameParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(FlameParticle), (const void*)4);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(FlameParticle), (const void*)16);
		glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(FlameParticle), (const void*)28);
	}
	static void disable()
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
};

template <typename Particle, int MaxParticlesCount, typename = typename
	::std::enable_if<::std::is_base_of<ParticleBase, Particle>::value>::type>
class Particles: public Object
{
	BEE_UNIFORM_GLOB(int, DeltaTimeMillis);
public:
	Particles()
	{
		createShader();
		initParticleSystem();
	}
	Particles(const ::glm::vec3 &position):
		fPosition(position)
	{
		createShader();
		initParticleSystem();
	}

	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<::glm::vec3, Types...>::value>::type>
	void setPosition(Types &&...args)
	{
		fPosition = ::glm::vec3(::std::forward<Types>(args)...);
		initParticleSystem();
	}
	::glm::vec3 getPosition() const 
	{
		return fPosition;
	}

	void render(ViewPort &viewPort) override
	{
		particleUpdater->use();
		int renderTime = glfwGetTime() * 1000;
		gDeltaTimeMillis = renderTime - prevRenderTime;
		prevRenderTime = renderTime;
		updateParticles();

		shader->use();
		setViewMatrices(viewPort);
		renderParticles();

		idxTFB = !idxTFB;
	}
protected:
	void initParticleSystem()
	{
		Particle particles[MaxParticlesCount];
		particles.[0].setPosition(fPosition);

		glGenTransformFeedbacks(2, &transformFeedbacks);
		glGenBuffers(2, particleBuffers);

		for (auto i = 0u; i != 2; ++i)
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbacks[i]);
			glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[i]);
			glBufferData(GL_ARRAY_BUFFER. sizeof(particles), particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffers[i]);
		}
	}
	void updateParticles()
	{
		glEnable(GL_RASTERIZER_DISCARD);
		
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[!idxTFB]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbacks[idxTFB]);

		Particle::enable();
		glBeginTransformFeedback(GL_POINTS);
		if (!initialize)
		{
			glDrawTransformFeedback(GL_POINTS, );
		}
		else
		{
			glDrawArrays(GL_POINTS, 0, 1);
			initialize = false;
		}
		glEndTransformFeedback();
		Particle::disable();

		glDisable(GL_RASTERIZER_DISCARD);
	}
	void renderParticles()
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer();
		glDrawTransformFeedback(GL_POINTS, particleBuffers[idxTFB]);
		glDisableVertexAttribArray(0);
	}
private:
	void createShader()
	{
		if (!particleUpdater)
		{
			shader = new Shader(
				VertexShader("particleUpdate-vs.glsl")
			);
		}
		if (!shader)
		{
			shader = new Shader(
				VertexShader("particles-vs.glsl"),
				FragmentShader("particles-fs.glsl")
			);
		}
	}
private:
	bool initialize = true;
	bool idxTFB = false;
	int prevRenderTime = glfwGetTime() * 1000;
	::glm::vec3 fPosition;
	GLuint transformFeedbacks[2], particleBuffers[2];
	static gl::Shader *particleUpdater, *shader;
};

using Flame = Particles<FlameParticle, 512>;

}