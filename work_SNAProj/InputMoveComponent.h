#pragma once
#include "ComponentBase.h"

class InputMoveComponent : public ComponentBase
{
public:
	InputMoveComponent(Actor * owner, float in_speedX = 100.0f, float in_speedY = 100.0f);
	~InputMoveComponent();

	void Update() override;

	enum AxisEnum
	{
		AxisEnum_x = 0,
		AxisEnum_y,
		AxisEnum_z
	};

	void SetHorizontalAxis(AxisEnum axis) { mHorizontalAxis = axis; }
	void SetVerticalAxis(AxisEnum axis) { mVerticalAxis = axis; }

	void SetHorizontalSpeed(float speed) { mSpeedHorizontal = speed; }
	void SetVerticalSpeed(float speed) { mSpeedVertical = speed; }

	float GetHorizontalSpeed() const { return mSpeedHorizontal; }
	float GetVerticalSpeed() const { return mSpeedVertical; }

private:
	float mSpeedHorizontal;
	float mSpeedVertical;

	char mHorizontalAxis;
	char mVerticalAxis;
};
