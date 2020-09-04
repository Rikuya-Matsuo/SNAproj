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
		size_t flameMass = mLifeAnimTextures[i].Load(System::GetInstance().GetRenderer(), "Assets/flame_parts1.png", 9, 3, 3, 1024, 1024, 0.1f);
	}

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/guide.png");
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
	// ‘Ì—ÍUI
	for (char i = 0; i < mPlayer->GetLife(); ++i)
	{
		Texture * tex = mLifeAnimTextures[i].GetCurrentTexture();
		if (!tex)
		{
			printf("Return is null...\n");
		}

		float scale = 0.3f;

		Vector2D pos;
		pos.x = -(System::GetInstance().GetRenderer()->GetScreenWidth() / 2) + (tex->GetWidth() / 3 * scale / 2) + (tex->GetWidth() / 3 * scale * i);
		pos.y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2 - tex->GetHeight() * scale / 2 + tex->GetHeight() * scale / 5;

		DrawTexture(shader, tex, pos, scale);
	}

	// ƒKƒCƒhUI
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
