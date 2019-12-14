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

	virtual void Update();

	void SetPosition(const Vector3D & pos) { mPosition = pos; }
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

	float GetFallSpeedRate() const { return mFallSpeedRate; }

	virtual void OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt);
	virtual void OnTouching(const ColliderComponentBase * caller, ColliderAttribute colAtt);
	virtual void OnApart(const ColliderComponentBase * caller, ColliderAttribute colAtt);

protected:
	// ビットフラグとして使う型。ここを書き換えることでサイズを一括で変えれる！
	typedef Uint8 FlagType;
	static const FlagType mRequestComponentSortMask;
	static const FlagType mStopDrawFlagMask;
	static const FlagType mBeyondSceneFlagMask;
	static const FlagType mAffectGravityFlagMask;
	static const FlagType mMovalFlagMask;


	FlagType mFlags;

	std::list<ComponentBase *> mComponentList;

	Vector3D mPosition;

	Vector3D mMoveVector;

	Vector3D mLimitSpeed;

	Matrix4 mWorldTransform;

	Quaternion mRotation;

	float mScale;

	float mFallSpeedRate;

	void UpdateComponents();

	void SortComponents();

	virtual void UpdateActor();

	void CalculateWorldTransform();

	void ClampSpeed();
};
