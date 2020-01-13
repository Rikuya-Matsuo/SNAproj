﻿#pragma once
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
		AxisEnum_z,
		AxisEnum_Invalid
	};

	void SetHorizontalAxis(AxisEnum axis) { mHorizontalAxis = axis; }
	void SetVerticalAxis(AxisEnum axis) { mVerticalAxis = axis; }

	void SetHorizontalSpeed(float speed) { mSpeedHorizontal = speed; }
	void SetVerticalSpeed(float speed) { mSpeedVertical = speed; }

	float GetHorizontalSpeed() const { return mSpeedHorizontal; }
	float GetVerticalSpeed() const { return mSpeedVertical; }

	bool GetHorizonInputFlag() const { return mInputFlags & (mLeftMask | mRightMask); }
	bool GetVerticalInputFlag() const { return mInputFlags & (mUpMask | mDownMask); }

	bool GetUpKey() const { return mInputFlags & mUpMask; }
	bool GetDownKey() const { return mInputFlags & mDownMask; }
	bool GetRightKey() const { return mInputFlags & mRightMask; }
	bool GetLeftKey() const { return mInputFlags & mLeftMask; }

private:
	typedef Uint8 FlagType;
	static const FlagType mLeftMask;
	static const FlagType mRightMask;
	static const FlagType mUpMask;
	static const FlagType mDownMask;

	FlagType mInputFlags;

	float mSpeedHorizontal;
	float mSpeedVertical;

	char mHorizontalAxis;
	char mVerticalAxis;
};
