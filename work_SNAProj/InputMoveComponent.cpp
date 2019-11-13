#include "InputMoveComponent.h"
#include "Input.h"


InputMoveComponent::InputMoveComponent(Actor * owner , float in_speedX, float in_speedY):
	ComponentBase(owner, 50),
	mSpeedX(in_speedX),
	mSpeedY(in_speedY)
{

}

InputMoveComponent::~InputMoveComponent()
{
}

void InputMoveComponent::Update()
{
	Vector2D nextPos = mOwner->GetPosition();

	if (Input::GetInstance().GetKey(SDL_SCANCODE_LEFT))
	{
		nextPos.x -= mSpeedX;
	}
	
	if (Input::GetInstance().GetKey(SDL_SCANCODE_RIGHT))
	{
		nextPos.x += mSpeedX;
	}

	if (Input::GetInstance().GetKey(SDL_SCANCODE_UP))
	{
		nextPos.y -= mSpeedY;
	}

	if (Input::GetInstance().GetKey(SDL_SCANCODE_DOWN))
	{
		nextPos.y += mSpeedY;
	}

	mOwner->SetPosition(nextPos);
}
