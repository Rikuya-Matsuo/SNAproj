#include "TitleUI.h"
#include "System.h"
#include "Texture.h"

TitleUI::TitleUI()
{
	mLogo = System::GetInstance().GetRenderer()->GetTexture("Assets/TitleLogo.png");

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/titleGuide.png");
}

TitleUI::~TitleUI()
{
}

void TitleUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mLogo);

	Vector2D pos;
	pos.x = 0.0f;
	pos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, pos);
}
