#pragma once
#include "SceneBase.h"

class TestScene : public SceneBase
{
public:
	TestScene();
	~TestScene();

	void Update() override;

private:
	void GenerateBGWall(float height);

	void GenerateBGDecoration(float height);
};
