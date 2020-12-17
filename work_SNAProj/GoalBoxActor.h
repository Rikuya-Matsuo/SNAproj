#pragma once
#include "Actor.h"

class GameScene;

class GoalBoxActor : public Actor
{
public:
	GoalBoxActor();
	~GoalBoxActor();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
