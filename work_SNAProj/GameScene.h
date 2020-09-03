#pragma once
#include "SceneBase.h"

class Player;
class LifeUIManager;
class UIScreen;

class GameScene : public SceneBase
{
public:
	GameScene();
	~GameScene();

	void Update() override;

private:
	Player * mPlayer;

	//LifeUIManager * mLifeUI;

	void GenerateBGWall(float height);

	void GenerateBGDecoration(float height);

	UIScreen * mUI;
};
