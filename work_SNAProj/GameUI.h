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

	enum JewelColor : Uint8
	{
		Green = 0,
		Yellow,
		Red,
		Invalid
	};
	Texture * mLifeJewelTextures[JewelColor::Invalid];

	JewelColor mCurrentJewelColor;

	Texture * mInochiTexture;

	Texture * mGuide;

	NinjaArtsUICircle * mNinjaArtsUI;
};
