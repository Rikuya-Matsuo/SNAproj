#pragma once
#include "Actor.h"

class SkeletalMeshComponent;

class Player : public Actor
{
public:
	Player();
	~Player();

	void Update() override;

private:
	void UpdateActor() override;

	SkeletalMeshComponent * mMeshComponent;

	void OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;

	void OnTouching(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;

	void OnApart(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;
};
