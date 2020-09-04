#pragma once
#include "SceneBase.h"

class UIScreen;

class GameOverScene : public SceneBase
{
public:
	GameOverScene();
	~GameOverScene();
	
	void Update() override;

private:
	UIScreen * mUI;
};
