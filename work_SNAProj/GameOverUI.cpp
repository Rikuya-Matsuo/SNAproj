#include "GameOverUI.h"
#include "System.h"
#include "Texture.h"

GameOverUI::GameOverUI()
{
	// 画面中央に表示する画像
	mGraph = System::GetInstance().GetRenderer()->GetTexture("Assets/GameOver.png");

	// ガイドの画像
	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/resultGuide.png");
}

GameOverUI::~GameOverUI()
{
}

void GameOverUI::Draw(Shader * shader) const
{
	// 中央画像の描画
	DrawTexture(shader, mGraph, Vector2D::zero, 2.0f);

	// ガイドの表示位置
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	// ガイド描画
	DrawTexture(shader, mGuide, guidePos);
}
