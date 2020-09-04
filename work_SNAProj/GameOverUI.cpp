#include "GameOverUI.h"
#include "System.h"

GameOverUI::GameOverUI()
{
	mGraph = System::GetInstance().GetRenderer()->GetTexture("Assets/GameOver.png");
}

GameOverUI::~GameOverUI()
{
}

void GameOverUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mGraph, Vector2D::zero, 2.0f);
}
