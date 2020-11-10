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
const Actor::FlagType Actor::mFixXScaleFlagMask_Base				= 1 << 9;
const Actor::FlagType Actor::mFixYScaleFlagMask_Base				= 1 << 10;
const Actor::FlagType Actor::mFixZScaleFlagMask_Base				= 1 << 11;

Actor::Actor():
	mPosition(Vector3D::zero),
	mPositionBeforeMove(Vector3D::zero),
	mMoveVector(Vector3D::zero),
	mRotationAxis(Vector3D(0.0f, 0.0f, 1.0f)),
	mFixVector(Vector3D::zero),
	mFixedScale(Vector3D::zero),
	mPushedVector(Vector3D::zero),
	mPriority(0),
	mRotationAngle(0.0f),
	mScale(1.0f),
	mFallSpeedRate(1.0f),
	mFallSpeedMax(30.0f),
	mFlags(mAffectGravityFlagMask_Base | mMovalFlagMask_Base | mCalculateTransformFlagMask_Base)
{
	// 自身をシステムに登録
	System::GetInstance().ResisterActor(this);

	// 前フレーム情報を初期化
	mPrevFlags = mFlags;
	mPrevRotationAngle = mRotationAngle;

	// 所属シーンを記録
	mBelongScene = const_cast<SceneBase*>(SceneBase::GetLatestScene());
}

Actor::~Actor()
{
	// コンポーネントの削除
	for (auto cmpList : mComponentLists)
	{
		Common::DeleteContainerOfPointer(mComponentLists[cmpList.first]);
		std::list<ComponentBase *>().swap(mComponentLists[cmpList.first]);
	}

	// システムから自身を登録解除
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

	// 回転時の処理
	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;

		mPrevRotationAngle = mRotationAngle;
	}

	// 動いた際の行列計算
	if (mFlags & mCalculateTransformFlagMask_Base)
	{
		CalculateWorldTransform();

		mFlags &= ~mCalculateTransformFlagMask_Base;
	}

	// アクティブであるとき、更新
	bool isActive = !(mFlags & mStopUpdateFlagMask_Base);
	if (isActive)
	{
		UpdateActor0();
	}

	// 回転時の処理
	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;

		mPrevRotationAngle = mRotationAngle;
	}

	// コンポーネントの処理
	UpdateComponents(UpdateTiming::UpdateTiming_Normal);

	// アクティブであるとき、更新（コンポーネントの更新後にしたい処理）
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

	// UpdateActor1の後にしたいコンポーネントの処理
	UpdateComponents(UpdateTiming::UpdateTiming_AfterAddMoveVector);

	// アップデート中にアクティブでなくなる可能性があるのでもう一度判定・処理を行う
	bool getNonActive_after = !(mPrevFlags & mStopUpdateFlagMask_Base) && mFlags & mStopUpdateFlagMask_Base;
	if (!getNonActive && getNonActive_after)
	{
		OnBecomeNotActive();
	}

	// 回転時の処理
	if (mPrevRotationAngle != mRotationAngle)
	{
		mRotation = Quaternion(mRotationAxis, mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;
		
		mPrevRotationAngle = mRotationAngle;
	}

	// 動いた際の行列処理
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
	// 所属シーンを更新
	mBelongScene = const_cast<SceneBase*>(SceneBase::GetLatestScene());
}

void Actor::ResisterComponent(const ComponentBase * in_cmp)
{
	// コンポーネントの更新優先度数を取得
	const int priority = in_cmp->GetPriority();

	// 更新のタイミングを取得
	const UpdateTiming timing = in_cmp->GetUpdateTiming();

	// 更新順に並ぶように、リストに挿入する
	for (auto itr : mComponentLists[timing])
	{
		if (priority < itr->GetPriority())
		{
			auto insertPoint = std::find(mComponentLists[timing].begin(), mComponentLists[timing].end(), itr);
			mComponentLists[timing].insert(insertPoint, const_cast<ComponentBase*>(in_cmp));
			return;
		}
	}

	// 上の処理で挿入されなかった場合、最後に挿入する
	mComponentLists[timing].emplace_back(const_cast<ComponentBase*>(in_cmp));
}

void Actor::DeresisterComponent(const ComponentBase * in_cmp)
{
	// 更新タイミングを取得
	const UpdateTiming timing = in_cmp->GetUpdateTiming();

	// 対象を検索
	auto target = std::find(mComponentLists[timing].begin(), mComponentLists[timing].end(), const_cast<ComponentBase*>(in_cmp));

	// 検索がヒットした場合、リストから削除する
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

	// 更新
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
	// 各軸のスケール値固定フラグを取得
	bool fixX = mFlags & mFixXScaleFlagMask_Base;
	bool fixY = mFlags & mFixYScaleFlagMask_Base;
	bool fixZ = mFlags & mFixZScaleFlagMask_Base;
	
	// スケール値固定フラグに応じて、各軸のスケール値を設定
	float xScale = (fixX) ? mFixedScale.x : mScale;
	float yScale = (fixY) ? mFixedScale.y : mScale;
	float zScale = (fixZ) ? mFixedScale.z : mScale;

	// 行列の計算
	mWorldTransform = Matrix4::CreateScale(xScale, yScale, zScale);

	mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);

	mWorldTransform *= Matrix4::CreateTranslation(mPosition);
}

void Actor::SetPriority(int value)
{
	// アクター更新順を更新
	mPriority = value;

	// システムにアクターのソートを要請する
	System::GetInstance().RequestSortActor();
}

void Actor::SetFixScaleFlag(bool x, bool y, bool z)
{
	// 可読性のためのラムダ式
	auto set = [this](bool flag, FlagType mask)
	{
		BitFlagFunc::SetFlagByBool(flag, this->mFlags, mask);
	};

	// 各軸のスケール値固定フラグを設定
	set(x, mFixXScaleFlagMask_Base);
	set(y, mFixYScaleFlagMask_Base);
	set(z, mFixZScaleFlagMask_Base);
}

void Actor::OnBecomeNotActive()
{
	// コンポーネントを一時休止する
	SleepAllComponent();
}

void Actor::OnBecomeActive()
{
	// コンポーネントを再起動する
	WakeAllComponent();
}

void Actor::SetAllComponentActive(bool active)
{
	// 全コンポーネントのアクティブ状態を設定する
	for (auto list : mComponentLists)
	{
		for (auto cmp : list.second)
		{
			cmp->SetActive(active);
		}
	}
}

void Actor::SleepAllComponent()
{
	// 全コンポーネントの一時休止
	for (auto list : mComponentLists)
	{
		for (auto cmp : list.second)
		{
			cmp->Sleep();
		}
	}
}

void Actor::WakeAllComponent()
{
	// 全コンポーネントを再起動する
	for (auto list : mComponentLists)
	{
		for (auto cmp : list.second)
		{
			cmp->Wake();
		}
	}
}

void Actor::SetFixVector(const Vector3D & vec)
{
	// 押し返しベクトルの取得
	mPushedVector = vec;

	// 総合的な押し返しベクトルを計算
	mFixVector += mPushedVector;
}

void Actor::FixPosition()
{
	// 押し返しが発生している場合処理を行う
	if (mFixVector.LengthSq())
	{
		// 位置の調整
		mPosition += mFixVector;

		// 行列計算
		CalculateWorldTransform();
		
		// 可読性のためのラムダ式
		auto isMinus = [](float value)
		{
			return value < 0.0f;
		};

		// 各軸の移動の制限
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

		// 押し返しベクトルの初期化
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
