#pragma once
#include "Actor.h"

class Mesh;

class EnemyBase : public Actor
{
public:
	EnemyBase(unsigned char lifeMax = 5);
	~EnemyBase();

	const char mLifeMax;

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

protected:
	typedef Uint8 FlagType;
	static const FlagType mAliveFlagMask;
	static const FlagType mFindPlayerFlagMask;
	static const FlagType mImmortalFlagMask;


	FlagType mFlags_Enemy;

	char mLife;

	Mesh * mMesh;

	virtual void UpdateEnemy0() = 0;

	virtual void UpdateEnemy1() = 0;

	virtual void OnLifeRunOut();



private:
	void UpdateActor0() final override;

	void UpdateActor1() final override;


};
