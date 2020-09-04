#pragma once
#include "UIScreen.h"

class GameOverUI : public UIScreen
{
public:
	GameOverUI();
	~GameOverUI();

	void Draw(Shader * shader) const override;

private:
	Texture * mGraph;
};

