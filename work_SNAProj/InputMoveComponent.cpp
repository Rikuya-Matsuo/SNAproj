#include "InputMoveComponent.h"
#include "Input.h"
#include "System.h"

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
	Vector3D nextPos = mOwner->GetPosition();

	float speedX = mSpeedX * System::GetInstance().GetDeltaTime();
	float speedY = mSpeedY * System::GetInstance().GetDeltaTime();

	if (Input::GetInstance().GetGamePadButton(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_LEFT))
	{
		nextPos.x -= speedX;
	}
	
	if (Input::GetInstance().GetGamePadButton(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_RIGHT))
	{
		nextPos.x += speedX;
	}

	if (Input::GetInstance().GetGamePadButton(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_UP))
	{
		nextPos.y -= speedY;
	}

	if (Input::GetInstance().GetGamePadButton(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_DOWN))
	{
		nextPos.y += speedY;
	}

	mOwner->SetPosition(nextPos);
}
