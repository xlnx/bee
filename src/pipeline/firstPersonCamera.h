#pragma once

#include "viewPort.h"
#include "window.h"

namespace bee
{

template <int cursorSensitivityLevels = 10>
class FirstPersonCamera: public ViewPort
{
	static_assert(cursorSensitivityLevels >= 5, "cursor sensitive level muse be >= 5");
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<ViewPort, Types...>::value>::type >
	FirstPersonCamera(Types &&...args): 
		ViewPort(::std::forward<Types>(args)...)
	{
		GLWindowBase::dispatch<CursorPosEvent>(
			[this](double x, double y) -> bool {
				auto detX = x - GLWindowBase::getCursorX();
				auto detY = y - GLWindowBase::getCursorY();

				auto angleH = detX * cursorStep;
				auto angleV = detY * cursorStep;

				update(-angleH, angleV);
				return false;
			}, cursorPosPriority
		);
		GLWindowBase::dispatch<RenderEvent>(
			[this]() -> bool {
				auto shouldUpdate = false;
				auto angleH = 0.f, angleV = 0.f;
				if (GLWindowBase::getCursorX() == 0) {
					angleH = -cursorStep; shouldUpdate = true;
				} else if (GLWindowBase::getCursorX() == GLWindowBase::getWidth() - 1) {
					angleH = cursorStep; shouldUpdate = true;
				}
				if (GLWindowBase::getCursorY() == 0) {
					angleV = -cursorStep; shouldUpdate = true;
				} else if (GLWindowBase::getCursorY() == GLWindowBase::getHeight() - 1) {
					angleV = cursorStep; shouldUpdate = true;
				}
				if (shouldUpdate) {
					update(-angleH * 2, angleV * 2);
				}
				return false;
			}, cursorPosPriority
		);
		setCursorSensitivity(cursorSensitivity);
	}
	void setCursorSensitivity(int value)
	{
		if (value < 0 || value > cursorSensitivityLevels)
		{
			BEE_RAISE(Fatal, "Invalid cursor sensitivity value");
		}
		cursorSensitivity = value;
		cursorStep = cursorStepBase * (float(value) + cursorSensitivityLevels / 2) / cursorSensitivityLevels;
	}
	int getCursorSensitivity() const
	{
		return cursorSensitivity;
	}
protected:
	void update(float angleH, float angleV)
	{
		auto sinx = sinf(angleH / 2);
		auto cosx = cosf(angleH / 2);
		auto siny = sinf(angleV / 2);
		auto cosy = cosf(angleV / 2);
		
		auto up = getUp();
		auto n = getTarget();
		auto left = ::glm::cross(up, n);
		
		setTarget(::glm::rotate(
			::glm::quat(cosy, left.x * siny, left.y * siny, left.z * siny), 
			::glm::rotate(::glm::quat(cosx, up.x * sinx, up.y * sinx, up.z * sinx), n))
		);
	}
protected:
	static constexpr auto cursorStepBase = .002f;
	float cursorStep;
	int cursorSensitivity = cursorSensitivityLevels / 2;
public:
	static constexpr int cursorPosPriority = 0;
};

}