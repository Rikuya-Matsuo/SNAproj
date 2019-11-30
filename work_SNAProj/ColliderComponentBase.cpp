#include "ColliderComponentBase.h"
#include "PhysicManager.h"

ColliderComponentBase::ColliderComponentBase(Actor * owner, ColliderAttribute colAtt, ColliderShape colShape) :
	ComponentBase(owner, 300),
	mAttribute(colAtt),
	mShape(colShape),
	mHitReaction(nullptr)
{
	if (mAttribute == ColliderAttribute::ColAtt_Detector)
	{
		mPriority -= 50;
		mOwner->RequestSortComponents();
	}

	PhysicManager::GetInstance().ResisterCollider(this);
}

ColliderComponentBase::~ColliderComponentBase()
{
	PhysicManager::GetInstance().DeresisterCollider(this);
}
