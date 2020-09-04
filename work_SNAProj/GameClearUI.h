#pragma once
#include "UIScreen.h"

class GameClearUI : public UIScreen
{
public:
	GameClearUI();
	~GameClearUI();

	void Draw(Shader * shader) const override;

private:
	Texture * mGraph;

	Texture * mGuide;
};
