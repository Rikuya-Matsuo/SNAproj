#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "BitFlagFunc.h"
#include <list>

class ComponentBase;
class ColliderComponentBase;
enum ColliderAttribute;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void Update() final;

	void SetPosition(const Vector3D & pos) { mPosition = pos; mFlags |= mCalculateTransformFlagMask; }
	const Vector3D& GetPosition() const { return mPosition; }

	void SetMoveVector(const Vector3D & vec) { mMoveVector = vec; }
	const Vector3D& GetMoveVector() const { return mMoveVector; }

	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	void SetRotation(const Quaternion & q) { mRotation = q; }
	const Quaternion& GetRotation() { return mRotation; }

	// コンポーネントの登録・登録解除
	void ResisterComponent(const ComponentBase * in_cmp);
	void DeresisterComponent(const ComponentBase * in_cmp);

	void RequestSortComponents() { mFlags |= mRequestComponentSortMask; }

	void SetVisible(bool value) { BitFlagFunc::SetFlagByBool(!value, mFlags, mStopDrawFlagMask); }
	bool GetVisibleFlag() const { return !(mFlags & mStopDrawFlagMask); }

	void SetBeyondSceneFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mBeyondSceneFlagMask); }
	bool GetBeyondSceneFlag() const { return mFlags & mBeyondSceneFlagMask; }

	void SetAffectGravityFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mAffectGravityFlagMask); }
	bool GetAffectGravityFlag() const { return mFlags & mAffectGravityFlagMask; }

	bool GetMovalFlag() const { return mFlags & mMovalFlagMask; }

	bool GetPlayerFlag() const { return mFlags & mPlayerFlagMask; }

	float GetFallSpeedRate() const { return mFallSpeedRate; }

	void SetFixVector(const Vector3D & vec);
	void FixPosition();

	virtual void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);
	virtual void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);
	virtual void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);

protected:
	// ビットフラグとして使う型。ここを書き換えることでサイズを一括で変えれる！
	typedef Uint8 FlagType;
	static const FlagType mRequestComponentSortMask;	// コンポーネントのソートを要請するフラグのマスク
	static const FlagType mStopDrawFlagMask;			// 描画をしないフラグのマスク
	static const FlagType mBeyondSceneFlagMask;			// シーンをまたいでもアクターの削除をスキップするフラグのマスク
	static const FlagType mAffectGravityFlagMask;		// 重力落下を行うかのフラグマスク（デフォルトで真）
	static const FlagType mMovalFlagMask;				// 何か物と当たったときに押し返しの影響を受けるかのフラグマスク（デフォルト真）
	static const FlagType mCalculateTransformFlagMask;	// 変形行列計算が必要であることのフラグマスク（デフォルトで真）
	static const FlagType mPlayerFlagMask;				// 本番ではいらないかも

	FlagType mFlags;

	std::list<ComponentBase *> mComponentList;

	Vector3D mPosition;

	Vector3D mMoveVector;

	// 秒間どのくらい動くかの各成分の制限値
	Vector3D mLimitSpeed;

	// 押し返し発生時の押し返しベクトル
	Vector3D mPushedVector;

	// 押し返しベクトルの総量
	Vector3D mFixVector;

	Matrix4 mWorldTransform;

	Quaternion mRotation;

	float mScale;

	float mFallSpeedRate;

	virtual void UpdateComponents() final;

	void SortComponents();

	virtual void UpdateActor0();

	// スピードのクランプと移動処理
	virtual void UpdateActor1();

	void CalculateWorldTransform();

	void ClampSpeed();
};
