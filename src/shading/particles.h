// TODO: modify this
#pragma once 

#include "shader.h"
#include "object.h"
#include "property.h"
#include "texture.h"

namespace bee
{

struct FlameParticle
{
	float type = 0.f;
	::glm::vec3 position = {0,0,0};
	::glm::vec3 velocity = {0,0,0.0001f};
	float lifeTimeMillis = 0.f;

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

	friend ::std::ostream & operator << (::std::ostream &out, const FlameParticle &particle)
	{
		out << particle.type << " (" << particle.position.x << "," << 
			particle.position.y << "," <<
			particle.position.z << ") (" << 
			particle.velocity.x << "," << 
			particle.velocity.y << "," << 
			particle.velocity.z << ") " <<
			particle.lifeTimeMillis;
		return out;
	}
};

template <typename Particle, int MaxParticlesCount>
class Particles: public Object
{
	BEE_UNIFORM_GLOB(float, DeltaTimeMillis);
	BEE_UNIFORM_GLOB(float, LauncherLifetime);
	BEE_UNIFORM_GLOB(float, ShellLifetime);
	BEE_UNIFORM_GLOB(float, SecondaryShellLifetime);
public:
	Particles()
	{
		initParticleSystem();
	}
	Particles(const ::glm::vec3 &position):
		fPosition(position)
	{
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

	void render(Viewport &viewport) override
	{
		particleUpdater->use();
		float renderTime = glfwGetTime() * 1000;
		gDeltaTimeMillis = renderTime - prevRenderTime;
		prevRenderTime = renderTime;
		gLauncherLifetime = 3000.f;
		gShellLifetime = 3000.f;
		gSecondaryShellLifetime = 3000.f;
		updateParticles();

		shader->use();
		setViewMatrices(viewport);
		renderParticles();
		shader->unuse();
		
		idxTFB = !idxTFB;
	}
protected:
	void initParticleSystem()
	{
		Particle particles[MaxParticlesCount];
		// particles[0].setPosition(fPosition);

		glGenTransformFeedbacks(2, transformFeedbacks);
		glGenBuffers(2, particleBuffers);

		for (auto i = 0u; i != 2; ++i)
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbacks[i]);
			glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffers[i]);
		}
	}
	void updateParticles()
	{
		glEnable(GL_RASTERIZER_DISCARD);
		randomTexture.invoke(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[!idxTFB]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbacks[idxTFB]);

		Particle::enable();
		
		GLuint query;
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
		glBeginTransformFeedback(GL_POINTS);
			if (!initialize)
			{
				glDrawTransformFeedback(GL_POINTS, transformFeedbacks[!idxTFB]);
			}
			else
			{
				glDrawArrays(GL_POINTS, 0, 1); initialize = false;
			}
		glEndTransformFeedback();
		gl::checkError();
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		gl::checkError();
		GLuint primitives;
		glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
		gl::checkError();
		BEE_LOG(primitives);
		Particle feedback[256];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
		gl::checkError();
		for (int i = 0; i != primitives; ++i)
		{
			BEE_LOG(feedback[i]);
		}
		// static int r = 0;
		// if (++r > 5) BEE_RAISE(Fatal, "asdasd");
		// gl::checkError();

		Particle::disable();
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glDisable(GL_RASTERIZER_DISCARD);
	}
	void renderParticles()
	{
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[idxTFB]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);
		glDrawTransformFeedback(GL_POINTS, transformFeedbacks[idxTFB]);
		glDisableVertexAttribArray(0);
	}
protected:
	bool idxTFB = true;
	bool initialize = true;
	float prevRenderTime = glfwGetTime() * 1000;
	::glm::vec3 fPosition;
	GLuint transformFeedbacks[2], particleBuffers[2];
	gl::Shader *shader = &gl::Shader::load(
		"particles",
		gl::VertexShader("billboard-vs.glsl"),
		gl::GeometryShader("billboard-gs.glsl"),
		gl::FragmentShader("billboard-fs.glsl")
	);
	gl::Shader *particleUpdater = &gl::Shader::loadTransformFeedback(
		"updateParticles",
		{ "Type1", "Position1", "Velocity1", "Age1" },
		gl::VertexShader("particleUpdate-vs.glsl"),
		gl::GeometryShader("particleUpdate-gs.glsl"),
		gl::FragmentShader("particleUpdate-fs.glsl")
	);
	gl::RandomTexture randomTexture = gl::RandomTexture(MaxParticlesCount);
};

using Flame = Particles<FlameParticle, 512>;

}