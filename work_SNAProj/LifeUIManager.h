#pragma once
#include "LifeUISprite.h"
class Player;

class LifeUIManager
{
public:
	LifeUIManager(Player * p);
	~LifeUIManager();

	void Update();

private:
	const char mLifeMax;

	Player * mPlayer;

	LifeUISprite ** mSprites;
};

