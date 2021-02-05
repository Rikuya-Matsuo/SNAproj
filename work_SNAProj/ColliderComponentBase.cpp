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
	// 検出装置として生成された場合
	if (mAttribute == ColliderAttribute::ColAtt_Detector)
	{
		mPriority -= 50;
		mFlags_CCBase &= ~(mMoveOnHitFlagMask | mPushOnHitFlagMask);
		mOwner->RequestSortComponents();
	}

	// 自身を物理マネージャに登録
	PhysicManager::GetInstance().ResisterCollider(this);
}

ColliderComponentBase::~ColliderComponentBase()
{
	// 物理マネージャから登録解除
	PhysicManager::GetInstance().DeresisterCollider(this);
}

void ColliderComponentBase::SetCheckOrder(int value)
{
	// 判定順を設定
	mCheckOrder = value;

	// ソートを要請
	PhysicManager::GetInstance().RequestSortCollider(mAttribute);
}

void ColliderComponentBase::ChangeAttribute(Uint8 newAtt)
{
	// 変更の必要がない場合、関数を終了
	if (mAttribute == newAtt)
	{
		return;
	}

	// 物理マネージャから登録解除
	PhysicManager::GetInstance().DeresisterCollider(this);

	// アトリビュート更新
	mAttribute = newAtt;

	// 物理マネージャに再登録
	PhysicManager::GetInstance().ResisterCollider(this);
}
