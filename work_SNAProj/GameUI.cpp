#include "GameUI.h"
#include "Player.h"
#include "AnimationChips.h"
#include "System.h"
#include "Texture.h"
#include "SceneBase.h"
#include <vector>
#include <cstdlib>

GameUI::GameUI(const Player * player):
	UIScreen(),
	mPlayer(player),
	mLifeMax(player->GetLife())
{
	mLifeAnimTextures = new AnimationChips[mLifeMax];
	for (char i = 0; i < mLifeMax; ++i)
	{
		const int frameMass = 9;

		mLifeAnimTextures[i].Load(System::GetInstance().GetRenderer(), "Assets/flame_parts1.png", frameMass, 3, 3, 1024, 1024, 0.1f);

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
}

GameUI::~GameUI()
{
	delete[] mLifeAnimTextures;
}

void GameUI::Update()
{
	UIScreen::Update();

	// （マルチスレッド読み込み実装に際して）シーンが読み込み中の場合は更新しない
	if (SceneBase::GetNowLoadingFlag())
	{
		return;
	}

	for (char i = 0; i < mLifeMax; ++i)
	{
		mLifeAnimTextures[i].Update();
	}
}

void GameUI::Draw(Shader * shader) const
{
	// （マルチスレッド読み込み実装に際して）シーンが読み込み中の場合は描画しない
	if (SceneBase::GetNowLoadingFlag())
	{
		return;
	}

	// 体力UI
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

	// ガイドUI
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
