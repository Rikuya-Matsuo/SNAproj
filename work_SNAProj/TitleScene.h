#pragma once
#include "SceneBase.h"

class Sprite;
class UIScreen;

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();

	void Update() override;

private:
	Sprite * mUITest;

	UIScreen * mUI;
};

