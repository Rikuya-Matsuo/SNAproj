#include "GameClearUI.h"
#include "System.h"
#include "Texture.h"

GameClearUI::GameClearUI()
{
	mGraph = System::GetInstance().GetRenderer()->GetTexture("Assets/GameClear.png");

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/resultGuide.png");
}

GameClearUI::~GameClearUI()
{
}

void GameClearUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mGraph, Vector2D::zero, 2.0f);

	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
