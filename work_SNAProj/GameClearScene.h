#pragma once
#include "SceneBase.h"

class UIScreen;

class GameClearScene : public SceneBase
{
public:
	GameClearScene();
	~GameClearScene();

	void Update() override;

private:
	UIScreen * mUI;
};
