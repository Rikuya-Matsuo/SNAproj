#include "Actor.h"
#include "System.h"
#include "Common.h"
#include "ComponentBase.h"
#include <algorithm>
#include <cmath>

const Actor::FlagType Actor::mRequestComponentSortMask = 1 << 0;
const Actor::FlagType Actor::mStopDrawFlagMask = 1 << 1;
const Actor::FlagType Actor::mBeyondSceneFlagMask = 1 << 2;
const Actor::FlagType Actor::mAffectGravityFlagMask = 1 << 3;
const Actor::FlagType Actor::mMovalFlagMask = 1 << 4;

Actor::Actor():
	mPosition(Vector3D::zero),
	mMoveVector(Vector3D::zero),
	mLimitSpeed(Vector3D(0.1f, 0.0f, 0.1f)),
	mFixVector(Vector3D::zero),
	mScale(1.0f),
	mFallSpeedRate(1.0f),
	mFlags(mAffectGravityFlagMask | mMovalFlagMask)
{
	System::GetInstance().ResisterActor(this);

	mFlags &= ~mAffectGravityFlagMask;
}

Actor::~Actor()
{
	Common::DeleteContainerOfPointer(mComponentList);
	std::list<ComponentBase *>().swap(mComponentList);

	System::GetInstance().DeresisterActor(this);
}

void Actor::Update()
{
	CalculateWorldTransform();

	UpdateComponents();

	UpdateActor();

	CalculateWorldTransform();
}

void Actor::ResisterComponent(const ComponentBase * in_cmp)
{
	const int priority = in_cmp->GetPriority();

	for (auto itr : mComponentList)
	{
		if (priority < itr->GetPriority())
		{
			auto insertPoint = std::find(mComponentList.begin(), mComponentList.end(), itr);
			mComponentList.insert(insertPoint, const_cast<ComponentBase*>(in_cmp));
			return;
		}
	}

	mComponentList.emplace_back(const_cast<ComponentBase*>(in_cmp));
}

void Actor::DeresisterComponent(const ComponentBase * in_cmp)
{
	auto target = std::find(mComponentList.begin(), mComponentList.end(), const_cast<ComponentBase*>(in_cmp));

	if (target != mComponentList.end())
	{
		mComponentList.erase(target);
	}
}

void Actor::UpdateComponents()
{
	// コンポーネントのソートがリクエストされていた場合はソートを行う
	if (mFlags & mRequestComponentSortMask)
	{
		SortComponents();

		// フラグを下す
		mFlags &= ~mRequestComponentSortMask;
	}

	for (auto component : mComponentList)
	{
		component->Update();
	}
}

void Actor::SortComponents()
{
	// 見よ！これがラムダ式を使ったソートである！
	mComponentList.sort([](ComponentBase * lhs, ComponentBase * rhs) { return lhs->GetPriority() < rhs->GetPriority(); });
}

void Actor::UpdateActor()
{
	ClampSpeed();

	mPosition += mMoveVector;
}

void Actor::CalculateWorldTransform()
{
	mWorldTransform = Matrix4::CreateScale(mScale);

	mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);

	mWorldTransform *= Matrix4::CreateTranslation(mPosition);
}

void Actor::ClampSpeed()
{
	auto clamp = [](float & speed, const float limit)
	{
		// 万が一limitに負の値が入っていた時のために絶対値を取っておく
		float limAbs = fabsf(limit);

		if (speed > limAbs)
		{
			speed = limAbs;
		}
		else if (speed < -limAbs)
		{
			speed = -limAbs;
		}
	};

	clamp(mMoveVector.x, mLimitSpeed.x);
	clamp(mMoveVector.y, mLimitSpeed.y);
	clamp(mMoveVector.z, mLimitSpeed.z);
}

void Actor::FixPosition()
{
	if (mFixVector.LengthSq() >= 1000.0f)
	{
		SDL_Delay(0);
	}

	mPosition += mFixVector;

	if (mFixVector.x)
	{
		mMoveVector.x = 0.0f;
	}
	else if (mFixVector.y)
	{
		mMoveVector.y = 0.0f;
	}
	else if (mFixVector.z)
	{
		mMoveVector.z = 0.0f;
	}

	mFixVector = Vector3D::zero;
}

void Actor::OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{

}

void Actor::OnTouching(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
}

void Actor::OnApart(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
}
