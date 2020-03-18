#include "ColliderComponentBase.h"
#include "PhysicManager.h"

const ColliderComponentBase::FlagType ColliderComponentBase::mMoveOnHitFlagMask = 1 << 0;
const ColliderComponentBase::FlagType ColliderComponentBase::mPushOnHitFlagMask = 1 << 1;
const ColliderComponentBase::FlagType ColliderComponentBase::mRotatableFlagMask = 1 << 2;

ColliderComponentBase::ColliderComponentBase(Actor * owner, ColliderAttribute colAtt, ColliderShape colShape) :
	ComponentBase(owner, 300, UpdateTiming::UpdateTiming_AfterAddMoveVector),
	mAttribute(colAtt),
	mShape(colShape),
	mFlags_CCBase(mMoveOnHitFlagMask | mPushOnHitFlagMask | mRotatableFlagMask),
	mCheckOrder(100)
{
	if (mAttribute == ColliderAttribute::ColAtt_Detector)
	{
		mPriority -= 50;
		mFlags_CCBase &= ~(mMoveOnHitFlagMask | mPushOnHitFlagMask);
		mOwner->RequestSortComponents();
	}

	PhysicManager::GetInstance().ResisterCollider(this);
}

ColliderComponentBase::~ColliderComponentBase()
{
	PhysicManager::GetInstance().DeresisterCollider(this);
}

void ColliderComponentBase::SetCheckOrder(int value)
{
	mCheckOrder = value;
	PhysicManager::GetInstance().RequestSortCollider(mAttribute);
}
