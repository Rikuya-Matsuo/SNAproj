#pragma once
#include "SceneBase.h"

class Player;
class LifeUIManager;

class TestScene : public SceneBase
{
public:
	TestScene();
	~TestScene();

	void Update() override;

private:
	Player * mPlayer;

	LifeUIManager * mLifeUI;

	void GenerateBGWall(float height);

	void GenerateBGDecoration(float height);
};
