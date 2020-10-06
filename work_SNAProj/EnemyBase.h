#pragma once
#include "Actor.h"

class Block;
class Stage;
class Mesh;
class EnemyManager;

class EnemyBase : public Actor
{
public:
	EnemyBase(unsigned char lifeMax = 5);
	~EnemyBase();

	const char mLifeMax;

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue);
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	bool GetAttackFlag() const { return mFlags_Enemy & mAttackFlagMask_EBase; }

	virtual void Capture();

	// Capture状態の解除
	virtual void LetGo();

	friend EnemyManager;

protected:
	typedef Uint8 FlagType;
	static const FlagType mAliveFlagMask_EBase;
	static const FlagType mImmortalFlagMask_EBase;
	static const FlagType mLookRightFlagMask_EBase;
	static const FlagType mAttackFlagMask_EBase;
	static const FlagType mBeCapturedFlagMask_EBase;

	static const float mDepth;

	FlagType mFlags_Enemy;

	char mLife;

	Mesh * mMesh;

	virtual void UpdateEnemy0() = 0;

	virtual void UpdateEnemy1() = 0;

	virtual void OnLifeRunOut();

	void Flip();

	virtual void OnFlip();

private:
	void UpdateActor0() final override;

	void UpdateActor1() final override;
};
