#include "GameUI.h"
#include "Player.h"
#include "AnimationChips.h"
#include "System.h"
#include "Texture.h"
#include "NinjaArtsUICircle.h"
#include <vector>
#include <cstdlib>

GameUI::GameUI(Player * player):
	UIScreen(),
	mPlayer(player),
	mLifeMax(player->GetLife()),
	mCurrentJewelColor(JewelColor::Green)
{
	// HP表示UI
	mLifeJewelTextures[JewelColor::Green] = System::GetInstance().GetRenderer()->GetTexture("Assets/magatamaGrn.png");
	mLifeJewelTextures[JewelColor::Yellow] = System::GetInstance().GetRenderer()->GetTexture("Assets/magatamaYel.png");
	mLifeJewelTextures[JewelColor::Red] = System::GetInstance().GetRenderer()->GetTexture("Assets/magatamaRed.png");

	// 「命」表示
	mInochiTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/Inochi.png");

	// ガイドの画像
	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/guide.png");

	// 忍術アイコンUI
	float radius = 60.0f;
	float halfScreenWidth = (System::GetInstance().GetRenderer()->GetScreenWidth() / 2);
	float halfScreenHeight = (System::GetInstance().GetRenderer()->GetScreenHeight() / 2);
	Vector2D pos;
	pos.x = (4 * halfScreenWidth) / 5;
	pos.y = -( 4 * halfScreenHeight) / 5;
	mNinjaArtsUI = new NinjaArtsUICircle(pos, radius);
	mPlayer->LinkNinjaArtsUICircle(mNinjaArtsUI);
	mNinjaArtsUI->SetIconScale(0.4f);
}

GameUI::~GameUI()
{
}

void GameUI::Update()
{
	// 基底クラス更新
	UIScreen::Update();

	// 忍術UI更新
	mNinjaArtsUI->Update();

	// プレイヤーの体力取得
	const char life = mPlayer->GetLife();
	if (life <= 3)
	{
		mCurrentJewelColor = JewelColor::Red;
	}
	else if (life <= 6)
	{
		mCurrentJewelColor = JewelColor::Yellow;
	}
}

void GameUI::Draw(Shader * shader) const
{
	// 画面の大きさを取得
	Vector2D screenSize(System::GetInstance().GetRenderer()->GetScreenWidth(), System::GetInstance().GetRenderer()->GetScreenHeight());

	// 「命」表示
	float inochiScale = 2.f;
	Vector2D inochiSize(static_cast<float>(mInochiTexture->GetWidth()), static_cast<float>(mInochiTexture->GetHeight()));
	Vector2D inochiRealSize = inochiSize * inochiScale;
	Vector2D inochiPos;
	inochiPos.x = (inochiRealSize.x / 2) - (screenSize.x / 2);
	inochiPos.y = -(inochiRealSize.y / 2) + (screenSize.y / 2);

	// 描画
	DrawTexture(shader, mInochiTexture, inochiPos, inochiScale);

	// 体力UI
	for (char i = 0; i < mPlayer->GetLife(); ++i)
	{
		// テクスチャ取得
		Texture * tex = mLifeJewelTextures[mCurrentJewelColor];
		if (!tex)
		{
			printf("GameUI Draw Function : Return is null...\n");
			return;
		}

		// 大きさ
		float scale = 1.0f;

		// 表示位置計算
		Vector2D texSize(static_cast<float>(tex->GetWidth()), static_cast<float>(tex->GetHeight()));
		Vector2D texRealSize = texSize * scale;
		Vector2D pos;
		pos.x = (texRealSize.x / 2) + (texRealSize.x * i) - (screenSize.x / 2) + inochiRealSize.x;
		pos.y = -(texRealSize.y / 2) + (screenSize.y / 2);

		// 描画
		DrawTexture(shader, tex, pos, scale);
	}

	// 忍術UI
	for (size_t i = 0; i < mNinjaArtsUI->GetTextures().size(); ++i)
	{
		Texture * tex = mNinjaArtsUI->GetTextures()[i];
		if (!tex)
		{
			// テクスチャがnullだった場合、スキップする
			continue;
		}

		// アイコンの表示位置を取得し、描画
		Vector2D pos;
		if (mNinjaArtsUI->GetPositionOf1Texture(i, pos))
		{
			DrawTexture(shader, tex, pos, mNinjaArtsUI->GetIconScale());
		}
	}
	// 忍術発動ボタンをUI上で表示(Yボタン)
	Texture * button = mNinjaArtsUI->GetButtonTexture_Y();
	DrawTexture(shader, button, mNinjaArtsUI->GetButtonUIPosition_Y(), mNinjaArtsUI->GetButtonUIScale());

	// (Xボタン)
	button = mNinjaArtsUI->GetButtonTexture_X();
	DrawTexture(shader, button, mNinjaArtsUI->GetButtonUIPosition_X(), mNinjaArtsUI->GetButtonUIScale());

	// ガイドUI
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;
	// 描画
	DrawTexture(shader, mGuide, guidePos);
}
