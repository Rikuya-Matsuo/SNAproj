#pragma once
#include "UIScreen.h"

class Player;
class AnimationChips;
class NinjaArtsUICircle;

class GameUI : public UIScreen
{
public:
	GameUI(Player * player);
	~GameUI();

	void Update() override;

	void Draw(Shader * shader) const override;

private:
	const char mLifeMax;

	Player * mPlayer;

	AnimationChips * mLifeAnimTextures;

	Texture * mGuide;

	NinjaArtsUICircle * mNinjaArtsUI;
};
