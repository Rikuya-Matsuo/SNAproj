#pragma once
#include "Block.h"

class Bomb : public Block
{
public:
	Bomb();
	~Bomb();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

private:
	BoxColliderComponent * mExplosionCollider;
};
