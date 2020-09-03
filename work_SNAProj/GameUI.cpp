#include "GameUI.h"
#include "Player.h"
#include "AnimationChips.h"
#include "System.h"
#include "Texture.h"

GameUI::GameUI(const Player * player):
	UIScreen(),
	mPlayer(player),
	mLifeMax(player->GetLife())
{
	mLifeAnimTextures = new AnimationChips[mLifeMax];
	for (char i = 0; i < mLifeMax; ++i)
	{
		size_t flameMass = mLifeAnimTextures[i].Load(System::GetInstance().GetRenderer(), "Assets/flame_parts1.png", 9, 3, 3, 1024, 1024, 0.05f);
		if (!flameMass)
		{
			printf("Fail to Load Animation chip...\n");
		}
	}
}

GameUI::~GameUI()
{
	delete[] mLifeAnimTextures;
}

void GameUI::Update()
{
	UIScreen::Update();

	for (char i = 0; i < mLifeMax; ++i)
	{
		mLifeAnimTextures[i].Update();
	}
}

void GameUI::Draw(Shader * shader) const
{
	for (char i = 0; i < mPlayer->GetLife(); ++i)
	{
		Texture * tex = mLifeAnimTextures[i].GetCurrentTexture();
		if (!tex)
		{
			printf("Return is null...\n");
		}

		float scale = 0.3f;

		float x = -(System::GetInstance().GetRenderer()->GetScreenWidth() / 2) + (tex->GetWidth() / 3 * scale / 2) + (tex->GetWidth() / 3 * scale * i);

		float y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2 - tex->GetHeight() * scale / 2 + tex->GetHeight() * scale / 5;

		Vector2D pos = Vector2D(x, y);

		DrawTexture(shader, tex, pos, scale);
	}
}
