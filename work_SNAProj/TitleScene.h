#pragma once
#include "SceneBase.h"

class Sprite;
class UIScreen;
class Stage;

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();

	void Update() override;

private:
	UIScreen * mUI;

	Stage * mStage;
};

