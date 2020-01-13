#include "InputMoveComponent.h"
#include "Input.h"
#include "System.h"

const InputMoveComponent::FlagType InputMoveComponent::mLeftMask = 1 << 0;
const InputMoveComponent::FlagType InputMoveComponent::mRightMask = 1 << 1;
const InputMoveComponent::FlagType InputMoveComponent::mUpMask = 1 << 2;
const InputMoveComponent::FlagType InputMoveComponent::mDownMask = 1 << 3;

InputMoveComponent::InputMoveComponent(Actor * owner , float in_speedX, float in_speedY):
	ComponentBase(owner, 50),
	mSpeedHorizontal(in_speedX),
	mSpeedVertical(in_speedY),
	mHorizontalAxis(AxisEnum_x),
	mVerticalAxis(AxisEnum_y),
	mInputFlags(0)
{

}

InputMoveComponent::~InputMoveComponent()
{
}

void InputMoveComponent::Update()
{
	Vector3D vec = mOwner->GetMoveVector();

	float speedHorizontal = mSpeedHorizontal * System::GetInstance().GetDeltaTime();
	float speedVertical = mSpeedVertical * System::GetInstance().GetDeltaTime();

	// エイリアス設定
	float * vecHorizontal = nullptr;
	float * vecVertical = nullptr;

	if (mHorizontalAxis == AxisEnum_x)
	{
		vecHorizontal = &vec.x;
	}
	else if (mHorizontalAxis == AxisEnum_y)
	{
		vecHorizontal = &vec.y;
	}
	else if (mHorizontalAxis == AxisEnum_z)
	{
		vecHorizontal = &vec.z;
	}

	if (mVerticalAxis == AxisEnum_x)
	{
		vecVertical = &vec.x;
	}
	else if (mVerticalAxis == AxisEnum_y)
	{
		vecVertical = &vec.y;
	}
	else if (mVerticalAxis == AxisEnum_z)
	{
		vecVertical = &vec.z;
	}

	// 操作受付。エイリアスを通じて移動ベクトルをいじる。
	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		|| Input::GetInstance().GetLStickX() < 0.0f
		|| Input::GetInstance().GetKey(SDL_SCANCODE_LEFT))
	{
		if (vecHorizontal)
		{
			*vecHorizontal -= speedHorizontal;
		}
		
		mInputFlags |= mLeftMask;
	}
	else
	{
		mInputFlags &= ~mLeftMask;
	}
	
	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		|| Input::GetInstance().GetLStickX() > 0.0f
		|| Input::GetInstance().GetKey(SDL_SCANCODE_RIGHT))
	{
		if (vecHorizontal)
		{
			*vecHorizontal += speedHorizontal;
		}

		mInputFlags |= mRightMask;
	}
	else
	{
		mInputFlags &= ~mRightMask;
	}

	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP)
		|| Input::GetInstance().GetLStickY() < 0.0f
		|| Input::GetInstance().GetKey(SDL_SCANCODE_UP))
	{
		if (vecVertical)
		{
			*vecVertical -= speedVertical;
		}

		mInputFlags |= mUpMask;
	}
	else
	{
		mInputFlags &= ~mUpMask;
	}

	if (Input::GetInstance().GetGamePadButtonPressed(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		|| Input::GetInstance().GetLStickY() > 0.0f
		|| Input::GetInstance().GetKey(SDL_SCANCODE_DOWN))
	{
		if (vecVertical)
		{
			*vecVertical += speedVertical;
		}

		mInputFlags |= mDownMask;
	}
	else
	{
		mInputFlags &= ~mDownMask;
	}

	// 位置情報の代入
	mOwner->SetMoveVector(vec);
}
