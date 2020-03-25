#include "LifeUIManager.h"
#include "Player.h"

LifeUIManager::LifeUIManager(Player * p):
	mPlayer(p),
	mLifeMax(p->GetLife())
{
	float scale = 40.0f;
	float height = 30.0f;
	Vector3D pos(-150, 0, height);
	Vector3D offset(scale / 3, 0, 0);

	mSprites = new LifeUISprite *[mLifeMax];
	for (char i = 0; i < mLifeMax; ++i)
	{
		mSprites[i] = new LifeUISprite;
		mSprites[i]->SetScale(scale);
		mSprites[i]->SetPosition(pos + (offset * i));
	}
}

LifeUIManager::~LifeUIManager()
{
}

void LifeUIManager::Update()
{
	char life = mPlayer->GetLife();

	if (life < 0)
	{
		return;
	}

	for (char i = 0; i < mLifeMax - life; ++i)
	{
		mSprites[mLifeMax - (i + 1)]->SetActive(false);
	}
}
