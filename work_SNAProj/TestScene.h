#pragma once
#include "SceneBase.h"

class Player;

class TestScene : public SceneBase
{
public:
	TestScene();
	~TestScene();

	void Update() override;

private:
	Player * mPlayer;

	void GenerateBGWall(float height);

	void GenerateBGDecoration(float height);
};
