#pragma once

#include "viewport.h"
#include "window.h"

namespace bee
{

class CameraCarrier: public WindowController
{
	static constexpr auto velocity = 0.05f;
public:
	CameraCarrier(Viewport &camera, float uVelocity = velocity, float vVelocity = velocity):
		camera(camera), uVelocity(uVelocity), vVelocity(vVelocity),
		updateEvent(
			GLWindowBase::dispatch<UpdateEvent>(
				[this](double detMillis)
				{
					auto forwardVec = this->camera.getTarget();
					auto leftwardVec = ::glm::normalize(::glm::cross(this->camera.getUp(), forwardVec));
				
					this->camera.setPosition(this->camera.getPosition() + 
						forwardVec * (forward -backward) * this->vVelocity * detMillis + 
						leftwardVec * (leftward - rightward) * this->uVelocity * detMillis);
					return false;
				}, priority
			)
		),
		keyEvent(
			GLWindowBase::dispatch<KeyEvent>(
				[this](int key, int scancode, int action, int mods)
				{
					switch (key)
					{
					case GLFW_KEY_UP: 
						forward = action != GLFW_RELEASE ? 1.f : 0.f; break;
					case GLFW_KEY_DOWN:
						backward = action != GLFW_RELEASE ? 1.f : 0.f; break;
					case GLFW_KEY_LEFT:
						leftward = action != GLFW_RELEASE ? 1.f : 0.f; break;
					case GLFW_KEY_RIGHT:
						rightward = action != GLFW_RELEASE ? 1.f : 0.f; break;
					}
					return false;
				}, priority
			)
		)
	{
	}
	~CameraCarrier()
	{
		updateEvent.cancel();
		keyEvent.cancel();
	}
private:
	float forward = 0.f;
	float leftward = 0.f;
	float rightward = 0.f;
	float backward = 0.f;

	Viewport &camera;
	float uVelocity, vVelocity;

	typename UpdateEvent::type &updateEvent;
	typename KeyEvent::type &keyEvent;
public:
	static constexpr int priority = 0;
};

}