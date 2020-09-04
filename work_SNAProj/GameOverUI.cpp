#include "GameOverUI.h"
#include "System.h"
#include "Texture.h"

GameOverUI::GameOverUI()
{
	mGraph = System::GetInstance().GetRenderer()->GetTexture("Assets/GameOver.png");

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/resultGuide.png");
}

GameOverUI::~GameOverUI()
{
}

void GameOverUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mGraph, Vector2D::zero, 2.0f);

	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
