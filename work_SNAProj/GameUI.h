#pragma once
#include "UIScreen.h"

class Player;
class AnimationChips;

class GameUI : public UIScreen
{
public:
	GameUI(const Player * player);
	~GameUI();

	void Update() override;

	void Draw(Shader * shader) const override;

private:
	const char mLifeMax;

	const Player * mPlayer;

	AnimationChips * mLifeAnimTextures;

	Texture * mGuide;
};
