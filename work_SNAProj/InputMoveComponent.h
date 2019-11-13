#pragma once
#include "ComponentBase.h"

class InputMoveComponent : public ComponentBase
{
public:
	InputMoveComponent(Actor * owner , float in_speedX = 1.0f, float in_speedY = 1.0f);
	~InputMoveComponent();

private:
	void Update() override;

	float mSpeedX;
	float mSpeedY;
};
