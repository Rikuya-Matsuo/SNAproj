#pragma once
#include "GameScene.h"

class GameSceneStage1 : public GameScene
{
public:
	GameSceneStage1();
	~GameSceneStage1();

private:
	void OnGoal() override;
};

