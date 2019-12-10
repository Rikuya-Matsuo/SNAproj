#include "InputMoveComponent.h"
#include "Input.h"
#include "System.h"

InputMoveComponent::InputMoveComponent(Actor * owner , float in_speedX, float in_speedY):
	ComponentBase(owner, 50),
	mSpeedHorizontal(in_speedX),
	mSpeedVertical(in_speedY),
	mHorizontalAxis(AxisEnum_x),
	mVerticalAxis(AxisEnum_y)
{

}

InputMoveComponent::~InputMoveComponent()
{
}

void InputMoveComponent::Update()
{
	Vector3D nextPos = mOwner->GetPosition();

	float speedHorizontal = mSpeedHorizontal * System::GetInstance().GetDeltaTime();
	float speedVertical = mSpeedVertical * System::GetInstance().GetDeltaTime();

	// エイリアス設定
	float * posHorizontal = &nextPos.x;
	float * posVertical = &nextPos.y;

	if (mHorizontalAxis == AxisEnum_y)
	{
		posHorizontal = &nextPos.y;
	}
	else if (mHorizontalAxis == AxisEnum_z)
	{
		posHorizontal = &nextPos.z;
	}

	if (mVerticalAxis == AxisEnum_x)
	{
		posVertical = &nextPos.x;
	}
	else if (mVerticalAxis == AxisEnum_z)
	{
		posVertical = &nextPos.z;
	}

	// 操作受付。エイリアスを通じて座標をいじる。
	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_LEFT))
	{
		*posHorizontal -= speedHorizontal;
	}
	
	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_RIGHT))
	{
		*posHorizontal += speedHorizontal;
	}

	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_UP))
	{
		*posVertical -= speedVertical;
	}

	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		|| Input::GetInstance().GetKey(SDL_SCANCODE_DOWN))
	{
		*posVertical += speedVertical;
	}

	// 位置情報の代入
	mOwner->SetPosition(nextPos);
}
