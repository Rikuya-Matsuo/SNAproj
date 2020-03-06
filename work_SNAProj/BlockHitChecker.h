#pragma once
#include "SDL/SDL.h"
#include "ComponentBase.h"

class Actor;
class BoxColliderComponent;

namespace BlockHitDirectionFlagMask
{
	static const Uint8 mUpMask		= 1 << 0;
	static const Uint8 mDownMask	= 1 << 1;
	static const Uint8 mRightMask	= 1 << 2;
	static const Uint8 mLeftMask	= 1 << 3;
	static const Uint8 mRUVerMask	= 1 << 4;
	static const Uint8 mLUVerMask	= 1 << 5;
	static const Uint8 mRDVerMask	= 1 << 6;
	static const Uint8 mLDVerMask	= 1 << 7;
}

class BlockHitChecker : public ComponentBase
{
public:
	Uint8 GetHitDirectionFlags() const { return mHitDirectionFlags; }

	BlockHitChecker(Actor * owner, BoxColliderComponent * box = nullptr);

	void SetBox(BoxColliderComponent * box) { mBox = box; }

	void Update();

	void SetOnHitDownFunction(void(*onHitDown)(Actor*)) { mOnHitDown = onHitDown; }

	void SetOnHitUpFunction(void(*onHitUp)(Actor*)) { mOnHitUp = onHitUp; }

	void SetOnHitRightFunction(void(*onHitRight)(Actor*)) { mOnHitRight = onHitRight; }

	void SetOnHitLeftFunction(void(*onHitLeft)(Actor*)) { mOnHitLeft = onHitLeft; }

private:
	Uint8 mHitDirectionFlags;

	BoxColliderComponent * mBox;

	void(*mOnHitDown)(Actor*);

	void(*mOnHitUp)(Actor*);

	void(*mOnHitRight)(Actor*);

	void(*mOnHitLeft)(Actor*);

	void CheckProcess();

	void PushProcess();
};
