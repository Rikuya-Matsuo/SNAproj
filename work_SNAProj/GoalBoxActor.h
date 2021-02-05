#pragma once
#include "Block.h"

class GameScene;

class GoalBoxActor : public Block
{
public:
	GoalBoxActor();
	~GoalBoxActor();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
