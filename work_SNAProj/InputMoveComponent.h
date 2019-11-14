#pragma once
#include "ComponentBase.h"

class InputMoveComponent : public ComponentBase
{
public:
	InputMoveComponent(Actor * owner, float in_speedX = 100.0f, float in_speedY = 100.0f);
	~InputMoveComponent();

private:
	void Update() override;

	float mSpeedX;
	float mSpeedY;
};
