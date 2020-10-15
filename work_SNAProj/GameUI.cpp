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
	mLifeMax(player->GetLife())
{
	mLifeAnimTextures = new AnimationChips[mLifeMax];
	for (char i = 0; i < mLifeMax; ++i)
	{
		const int frameMass = 9;

		mLifeAnimTextures[i].Load(System::GetInstance().GetRenderer(), "Assets/flame_parts1.png", frameMass, 3, 3, 1024, 1024, 0.05f);

		const bool useRandonFlame = true;
		if (useRandonFlame)
		{
			// アニメーションチップの順番を個体ごとのランダムにする
			std::vector<int> routine;

			// 最初のフレームのインデックスを設定
			int tmp = rand() % frameMass;
			routine.emplace_back(tmp);

			// 上で設定したフレームと合わせて、6~8フレームのルーチンとする
			int routineFrameMass = 5 + rand() % 3;

			for (int j = 0; j < routineFrameMass; ++j)
			{
				// 前のフレームより1~2フレーム進めたものをインデックスとして設定する
				tmp += 1 + rand() % 2;

				// tmpがフレーム数を超えて入れば、それ以内に収める
				tmp %= frameMass;

				// 設定
				routine.emplace_back(tmp);
			}

			// routineに記録されたフレームの順番をmLifeAnimTextures[i]に登録
			mLifeAnimTextures[i].SetRoutine(routine.data(), routine.size());
		}
	}

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/guide.png");

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
	delete[] mLifeAnimTextures;
}

void GameUI::Update()
{
	UIScreen::Update();

	for (char i = 0; i < mLifeMax; ++i)
	{
		mLifeAnimTextures[i].Update();
	}

	mNinjaArtsUI->Update();
}

void GameUI::Draw(Shader * shader) const
{
	// 体力UI
	for (char i = 0; i < mPlayer->GetLife(); ++i)
	{
		Texture * tex = mLifeAnimTextures[i].GetCurrentTexture();
		if (!tex)
		{
			printf("GameUI Draw Function : Return is null...\n");
			return;
		}

		float scale = 0.3f;

		Vector2D pos;
		pos.x = -(System::GetInstance().GetRenderer()->GetScreenWidth() / 2) + (tex->GetWidth() / 3 * scale / 2) + (tex->GetWidth() / 3 * scale * i);
		pos.y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2 - tex->GetHeight() * scale / 2 + tex->GetHeight() * scale / 5;

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

		Vector2D pos;
		if (mNinjaArtsUI->GetPositionOf1Texture(i, pos))
		{
			DrawTexture(shader, tex, pos, mNinjaArtsUI->GetIconScale());
		}
	}
	Texture * button = mNinjaArtsUI->GetButtonTexture();
	DrawTexture(shader, button, mNinjaArtsUI->GetButtonUIPosition(), mNinjaArtsUI->GetButtonUIScale());

	// ガイドUI
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
