#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "BitFlagFunc.h"
#include <list>
#include <unordered_map>

class ComponentBase;
class ColliderComponentBase;
class SceneBase;
enum ColliderAttribute;
enum UpdateTiming;

class Actor
{
protected:
	// ビットフラグとして使う型。ここを書き換えることでサイズを一括で変えれる！
	typedef Uint16 FlagType;
	static const FlagType mRequestComponentSortMask_Base;		// コンポーネントのソートを要請するフラグのマスク
	static const FlagType mStopDrawFlagMask_Base;				// 描画をしないフラグのマスク
	static const FlagType mBeyondSceneFlagMask_Base;			// シーンをまたいでもアクターの削除をスキップするフラグのマスク
	static const FlagType mAffectGravityFlagMask_Base;			// 重力落下を行うかのフラグマスク（デフォルトで真）
	static const FlagType mMovalFlagMask_Base;					// 何か物と当たったときに押し返しの影響を受けるかのフラグマスク（デフォルト真）
	static const FlagType mCalculateTransformFlagMask_Base;		// 変形行列計算が必要であることのフラグマスク（デフォルトで真）
	static const FlagType mStopUpdateFlagMask_Base;				// 更新を止めるか否かのフラグマスク
	static const FlagType mInCameraFlagMask_Base;
	static const FlagType mBuryDeeplyFlagMask_Base;
	static const FlagType mFixXScaleFlagMask_Base;
	static const FlagType mFixYScaleFlagMask_Base;
	static const FlagType mFixZScaleFlagMask_Base;

	FlagType mFlags;

	typedef std::list<ComponentBase *> ComponentList;
	std::unordered_map<UpdateTiming, ComponentList> mComponentLists;

	Vector3D mPosition;

	Vector3D mPositionBeforeMove;

	Vector3D mMoveVector;

	Vector3D mRotationAxis;
	float mRotationAngle;

	// 押し返し発生時の押し返しベクトル
	Vector3D mPushedVector;

	// 押し返しベクトルの総量
	Vector3D mFixVector;

	// スケールの各軸の固定値（各フラグが真のときのみ有効）
	Vector3D mFixedScale;

	Matrix4 mWorldTransform;

	Quaternion mRotation;

	// アップデートの順番
	// 値が小さい方が先に行う
	int mPriority;

	float mScale;

	float mFallSpeedRate;

	float mFallSpeedMax;

	SceneBase * mBelongScene;

	virtual void UpdateComponents(UpdateTiming timing) final;

	void SortComponents();

	virtual void UpdateActor0();

	// スピードのクランプと移動処理
	virtual void UpdateActor1();

	void CalculateWorldTransform();

	void SetPriority(int value);

	void SetFixScaleFlag(bool x, bool y, bool z);

	void SetFixedScale(const Vector3D& scale) { mFixedScale = scale; }

	virtual void OnBecomeNotActive();

	virtual void OnBecomeActive();

	void SetAllComponentActive(bool active);

	void SleepAllComponent();

	void WakeAllComponent();

public:
	Actor();
	virtual ~Actor();

	virtual void Update() final;

	void OnBeyondScene();

	const SceneBase * GetBelongScene() const { return mBelongScene; }

	void SetPosition(const Vector3D & pos) { mPosition = pos; mFlags |= mCalculateTransformFlagMask_Base; }
	const Vector3D& GetPosition() const { return mPosition; }

	void SetMoveVector(const Vector3D & vec) { mMoveVector = vec; }
	const Vector3D& GetMoveVector() const { return mMoveVector; }

	const Vector3D& GetRotationAxis() const { return mRotationAxis; }
	float GetRotationAngle() const { return mRotationAngle; }

	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	void SetRotation(const Quaternion & q) { mRotation = q; }
	const Quaternion& GetRotation() const { return mRotation; }

	void SetRotationAxis(const Vector3D & axis) { mRotationAxis = axis; }

	void SetRotationAngle(float angle) { mRotationAngle = angle; }

	// コンポーネントの登録・登録解除
	void ResisterComponent(const ComponentBase * in_cmp);
	void DeresisterComponent(const ComponentBase * in_cmp);

	void RequestSortComponents() { mFlags |= mRequestComponentSortMask_Base; }

	void SetVisible(bool value) { BitFlagFunc::SetFlagByBool(!value, mFlags, mStopDrawFlagMask_Base); }
	bool GetVisibleFlag() const { return !(mFlags & mStopDrawFlagMask_Base); }

	void SetBeyondSceneFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mBeyondSceneFlagMask_Base); }
	bool GetBeyondSceneFlag() const { return mFlags & mBeyondSceneFlagMask_Base; }

	void SetAffectGravityFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mAffectGravityFlagMask_Base); }
	bool GetAffectGravityFlag() const { return mFlags & mAffectGravityFlagMask_Base; }

	bool GetMovalFlag() const { return mFlags & mMovalFlagMask_Base; }

	void SetActive(bool value) { BitFlagFunc::SetFlagByBool(!value, mFlags, mStopUpdateFlagMask_Base); }
	bool GetActiveFlag() const { return !(mFlags & mStopUpdateFlagMask_Base); }

	void SetInCameraFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mInCameraFlagMask_Base); }
	bool GetInCameraFlag() const { return mFlags & mInCameraFlagMask_Base; }

	float GetFallSpeedRate() const { return mFallSpeedRate; }
	float GetFallSpeedMax() const { return mFallSpeedMax; }

	int GetPriority() const { return mPriority; }

	void SetFixVector(const Vector3D & vec);
	void FixPosition();

	virtual void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);
	virtual void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);
	virtual void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent);

	void OnBuryDeeply() { mFlags |= mBuryDeeplyFlagMask_Base; }

	FlagType GetBitFlag() const { return mFlags; }

	virtual bool IsPlayer() const { return false; }

private:
	FlagType mPrevFlags;

	float mPrevRotationAngle;
};
