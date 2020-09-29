﻿#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;
class JumpComponent;
class CompletionMeshActor;
class EnemyBase;
class AnimationEffect;
class NinjaArtsBase;

class Player final : public Actor
{
public:
	Player();
	~Player();

	bool IsPlayer() const override { return true; }

	bool GetAliveFlag() const { return mFlags_Player & mAliveFlagMask; }

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	bool GetLookRightFlag() const { return mFlags_Player & mLookRightFlagMask; }

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	static const char mLifeMax;

	friend NinjaArtsBase;

private:
	static const char mDashAttackPower;


	typedef Uint8 FlagType;
	static const FlagType mDetectWallFlagMask;
	static const FlagType mDetectGroundFlagMask;
	static const FlagType mLookRightFlagMask;
	static const FlagType mImmortalFlagMask;
	static const FlagType mAliveFlagMask;
	static const FlagType mKnockBackFlagMask;

	static const Vector3D mKnockBackVector;

	enum AnimationPattern
	{
		Anim_Stay = 0,
		Anim_DashAttack,
		Anim_Run,
		Anim_KnockBack,
		Anim_Invalid
	};

	Mesh * mMesh;

	InputMoveComponent * mInputComponent;

	BoxColliderComponent * mBoxCollider;

	BoxColliderComponent * mGroundChecker;

	BoxColliderComponent * mAttackCollider;

	BoxColliderComponent * mWallChecker;

	// ヒットした壁へのポインタを記録する変数
	// なぜ変数なのにconstが付いているのかということを考えると気持ち悪いが、
	// この位置のconstは「ポインタの指すインスタンスを弄らない」という約束みたいなもの。
	const ColliderComponentBase * mWallPointer;

	JumpComponent * mJumpComponent;

	CompletionMeshActor * mCompletionMeshActor;

	NinjaArtsBase * mCurrentCursorNinjaArts;

	char mCurrentAnimation;

	char mLife;
	
	// 押し返しが発生したフラグ。
	// 正負によって方向を表す。
	struct PushedFlag
	{
		char horizon : 4;
		char vertical : 4;

		void Init() { horizon = vertical = 0; }
	};
	PushedFlag mPushedFlags;

	FlagType mFlags_Player;

	FlagType mPrevFlags_Player;

	bool mPrevGravityFlag;

	std::list<EnemyBase *> mHitList;

	// 床として接触したブロックへのポインタのリスト
	std::list<const ColliderComponentBase *> mGroundList;

	const size_t mHitEffectMass;
	AnimationEffect ** mHitEffects;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnAttackColliderHits(const ColliderComponentBase * opponent);

	void OnGroundCheckerHits(const ColliderComponentBase * opponent);

	void OnWallCheckerHits(const ColliderComponentBase * opponent);

	void OnBodyHits(const ColliderComponentBase * opponent);

	void OnGroundCheckerTouching(const ColliderComponentBase * opponent);

	void OnWallCheckerTouching(const ColliderComponentBase * opponent);

	void OnBodyTouching(const ColliderComponentBase * opponent);

	// 地面検出装置が地面を検出した際の関数
	void OnDetectGround(const ColliderComponentBase * opponent);

	// 地面からの押し返しを受けた時の関数
	void OnLanding(const ColliderComponentBase * opponent);

	// 天井と衝突判定した時の関数
	void OnCollideCeiling(const ColliderComponentBase * opponent);

	void OnBePushedByWall();

	void OnBeAttacked(const EnemyBase * enemy);

	void OnLifeRunOut();

	// 攻撃終了時処理
	void OnEndAttack();

	AnimationEffect * FindNonActiveEffect(AnimationEffect ** effArray, size_t mass) const;
};
