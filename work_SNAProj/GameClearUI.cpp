#include "GameClearUI.h"
#include "System.h"

GameClearUI::GameClearUI()
{
	mGraph = System::GetInstance().GetRenderer()->GetTexture("Assets/GameClear.png");
}

GameClearUI::~GameClearUI()
{
}

void GameClearUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mGraph, Vector2D::zero, 2.0f);
}
