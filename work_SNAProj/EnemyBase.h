#pragma once
#include "Actor.h"

class Mesh;
class EnemyManager;

class EnemyBase : public Actor
{
public:
	EnemyBase(unsigned char lifeMax = 5);
	~EnemyBase();

	const char mLifeMax;

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	friend EnemyManager;

protected:
	typedef Uint8 FlagType;
	static const FlagType mAliveFlagMask_EBase;
	static const FlagType mFindPlayerFlagMask_EBase;
	static const FlagType mImmortalFlagMask_EBase;
	static const FlagType mLookRightFlagMask_EBase;

	static const float mDepth;

	FlagType mFlags_Enemy;

	char mLife;

	Mesh * mMesh;

	virtual void UpdateEnemy0() = 0;

	virtual void UpdateEnemy1() = 0;

	virtual void OnLifeRunOut();

	void Flip() { mRotationAngle = (mRotationAngle == 0.0f) ? static_cast<float>(M_PI) : 0.0f; }

private:
	void UpdateActor0() final override;

	void UpdateActor1() final override;


};
