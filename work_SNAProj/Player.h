#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;

class Player final : public Actor
{
public:
	Player();
	~Player();

	void Update() override;

private:
	Mesh * mMesh;

	InputMoveComponent * mInputComponent;

	void UpdateActor() override;

	void OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;

	void OnTouching(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;

	void OnApart(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;
};
