#include "TitleUI.h"
#include "System.h"
#include "Texture.h"

TitleUI::TitleUI()
{
	// ロゴ画像
	mLogo = System::GetInstance().GetRenderer()->GetTexture("Assets/TitleLogo.png");

	// ガイド画像
	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/titleGuide.png");
}

TitleUI::~TitleUI()
{
}

void TitleUI::Draw(Shader * shader) const
{
	// ロゴ描画
	DrawTexture(shader, mLogo);

	// ガイド画像表示位置
	Vector2D pos;
	pos.x = 0.0f;
	pos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;
	// 描画
	DrawTexture(shader, mGuide, pos);
}
