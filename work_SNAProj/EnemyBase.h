#pragma once
#include "Actor.h"

class Mesh;

class EnemyBase : public Actor
{
public:
	EnemyBase(char lifeMax = 5);
	~EnemyBase();

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	const char mLifeMax;

protected:
	typedef Uint8 FlagType;
	static const FlagType mAliveFlagMask;
	static const FlagType mFindPlayerFlagMask;


	FlagType mFlags_Enemy;

	char mLife;

	Mesh * mMesh;
};
