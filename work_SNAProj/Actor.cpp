#include "Actor.h"
#include "System.h"
#include "Common.h"
#include "ComponentBase.h"
#include "SceneBase.h"
#include <algorithm>
#include <cmath>

const Actor::FlagType Actor::mRequestComponentSortMask_Base			= 1 << 0;
const Actor::FlagType Actor::mStopDrawFlagMask_Base					= 1 << 1;
const Actor::FlagType Actor::mBeyondSceneFlagMask_Base				= 1 << 2;
const Actor::FlagType Actor::mAffectGravityFlagMask_Base			= 1 << 3;
const Actor::FlagType Actor::mMovalFlagMask_Base					= 1 << 4;
const Actor::FlagType Actor::mCalculateTransformFlagMask_Base		= 1 << 5;
const Actor::FlagType Actor::mStopUpdateFlagMask_Base				= 1 << 6;
const Actor::FlagType Actor::mInCameraFlagMask_Base					= 1 << 7;
const Actor::FlagType Actor::mBuryDeeplyFlagMask_Base				= 1 << 8;

Actor::Actor():
	mPosition(Vector3D::zero),
	mPositionBeforeMove(Vector3D::zero),
	mMoveVector(Vector3D::zero),
	mRotationAxis(Vector3D(0.0f, 0.0f, 1.0f)),
	mFixVector(Vector3D::zero),
	mPushedVector(Vector3D::zero),
	mPriority(0),
	mRotationAngle(0.0f),
	mScale(1.0f),
	mFallSpeedRate(1.0f),
	mFallSpeedMax(30.0f),
	mFlags(mAffectGravityFlagMask_Base | mMovalFlagMask_Base | mCalculateTransformFlagMask_Base)
{
	System::GetInstance().ResisterActor(this);

	mPrevFlags = mFlags;
	mPrevRotationAngle = mRotationAngle;

	mBelongScene = const_cast<SceneBase*>(SceneBase::GetLatestScene());
}

Actor::~Actor()
{
	for (auto cmpList : mComponentLists)
	{
		// secondを使って書きたかったが、なぜか参照が本体と同期していないため、添え字で指定する方法を使う
		Common::DeleteContainerOfPointer(mComponentLists[cmpList.first]);
		std::list<ComponentBase *>().swap(mComponentLists[cmpList.first]);
	}

	System::GetInstance().DeresisterActor(this);
}

void Actor::Update()
{
	// もし当たり判定で何かに深く埋まった場合、変な場所に飛び出すことを防止するため、前フレームのアップデート前の位置に戻す
	if (mFlags & mBuryDeeplyFlagMask_Base)
	{
		mPosition = mPositionBeforeMove;
		mMoveVector = Vector3D::zero;
		mFlags &= ~mBuryDeeplyFlagMask_Base;
	}
	// そうでなければ、アップデート実行前の位置ベクトルを記録
	else
	{
		mPositionBeforeMove = mPosition;
	}

	// アクティブ化した or アクティブでなくなった判定・処理
	bool getNonActive = !(mPrevFlags & mStopUpdateFlagMask_Base) && mFlags & mStopUpdateFlagMask_Base;
	bool getActive = mPrevFlags & mStopUpdateFlagMask_Base && !(mFlags & mStopUpdateFlagMask_Base);
	if (getNonActive)
	{
		OnBecomeNotActive();
	}
	else if (getActive)
	{
		OnBecomeActive();
	}

	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;

		mPrevRotationAngle = mRotationAngle;
	}

	if (mFlags & mCalculateTransformFlagMask_Base)
	{
		CalculateWorldTransform();

		mFlags &= ~mCalculateTransformFlagMask_Base;
	}

	bool isActive = !(mFlags & mStopUpdateFlagMask_Base);
	if (isActive)
	{
		UpdateActor0();
	}

	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;

		mPrevRotationAngle = mRotationAngle;
	}

	UpdateComponents(UpdateTiming::UpdateTiming_Normal);

	if (isActive)
	{
		UpdateActor1();
	}

	// 移動が発生しているなら移動させる
	if (mMoveVector.LengthSq())
	{
		mPosition += mMoveVector;

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	UpdateComponents(UpdateTiming::UpdateTiming_AfterAddMoveVector);

	// アップデート中にアクティブでなくなる可能性があるのでもう一度判定・処理を行う
	bool getNonActive_after = !(mPrevFlags & mStopUpdateFlagMask_Base) && mFlags & mStopUpdateFlagMask_Base;
	if (!getNonActive && getNonActive_after)
	{
		OnBecomeNotActive();
	}

	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;
		
		mPrevRotationAngle = mRotationAngle;
	}

	if (mFlags & mCalculateTransformFlagMask_Base)
	{
		CalculateWorldTransform();

		mFlags &= ~mCalculateTransformFlagMask_Base;
	}

	// フラグの記録
	mPrevFlags = mFlags;
}

void Actor::OnBeyondScene()
{
	mBelongScene = const_cast<SceneBase*>(SceneBase::GetLatestScene());
}

void Actor::ResisterComponent(const ComponentBase * in_cmp)
{
	const int priority = in_cmp->GetPriority();

	const UpdateTiming timing = in_cmp->GetUpdateTiming();

	for (auto itr : mComponentLists[timing])
	{
		if (priority < itr->GetPriority())
		{
			auto insertPoint = std::find(mComponentLists[timing].begin(), mComponentLists[timing].end(), itr);
			mComponentLists[timing].insert(insertPoint, const_cast<ComponentBase*>(in_cmp));
			return;
		}
	}

	mComponentLists[timing].emplace_back(const_cast<ComponentBase*>(in_cmp));
}

void Actor::DeresisterComponent(const ComponentBase * in_cmp)
{
	const UpdateTiming timing = in_cmp->GetUpdateTiming();

	auto target = std::find(mComponentLists[timing].begin(), mComponentLists[timing].end(), const_cast<ComponentBase*>(in_cmp));

	if (target != mComponentLists[timing].end())
	{
		mComponentLists[timing].erase(target);
	}
}

void Actor::UpdateComponents(UpdateTiming timing)
{
	// コンポーネントのソートがリクエストされていた場合はソートを行う
	if (mFlags & mRequestComponentSortMask_Base)
	{
		SortComponents();

		// フラグを下す
		mFlags &= ~mRequestComponentSortMask_Base;
	}

	for (auto component : mComponentLists[timing])
	{
		if (component != nullptr && component->GetActiveFlag())
		{
			component->Update();
		}
	}
}

void Actor::SortComponents()
{
	for (auto list : mComponentLists)
	{
		// ラムダ式を使ってソートする
		list.second.sort([](const ComponentBase * lhs, const ComponentBase * rhs) { return lhs->GetPriority() <= rhs->GetPriority(); });
	}
}

void Actor::UpdateActor0()
{
}

void Actor::UpdateActor1()
{
}

void Actor::CalculateWorldTransform()
{
	mWorldTransform = Matrix4::CreateScale(mScale);

	mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);

	mWorldTransform *= Matrix4::CreateTranslation(mPosition);
}

void Actor::SetPriority(int value)
{
	mPriority = value;
	System::GetInstance().RequestSortActor();
}

void Actor::OnBecomeNotActive()
{
	SetAllComponentActive(false);
}

void Actor::OnBecomeActive()
{
	// よくよく考えたらこの設計はよくないかもしれない
	// 以前アクターがアクティブだったときに、非アクティブだったコンポーネントもアクティブになる為。
	// あとで考える。
	SetAllComponentActive(true);
}

void Actor::SetAllComponentActive(bool active)
{
	for (auto list : mComponentLists)
	{
		for (auto cmp : list.second)
		{
			cmp->SetActive(active);
		}
	}
}

void Actor::SetFixVector(const Vector3D & vec)
{
	mPushedVector = vec;

	mFixVector += mPushedVector;
}

void Actor::FixPosition()
{
	if (mFixVector.LengthSq())
	{
		mPosition += mFixVector;

		CalculateWorldTransform();
		auto isMinus = [](float value)
		{
			return value < 0.0f;
		};

		if (mFixVector.x)
		{
			if (isMinus(mMoveVector.x) != isMinus(mFixVector.x))
			{
				mMoveVector.x = 0.0f;
			}
		}
		if (mFixVector.y)
		{
			if (isMinus(mMoveVector.y) != isMinus(mFixVector.y))
			{
				mMoveVector.y = 0.0f;
			}
		}
		if (mFixVector.z)
		{
			if (isMinus(mMoveVector.z) != isMinus(mFixVector.z))
			{
				mMoveVector.z = 0.0f;
			}
		}

		mFixVector = Vector3D::zero;
	}
}

void Actor::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{

}

void Actor::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}

void Actor::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}
