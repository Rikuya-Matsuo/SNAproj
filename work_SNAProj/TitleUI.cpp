#include "TitleUI.h"
#include "System.h"
#include "Texture.h"

TitleUI::TitleUI()
{
	mLogo = System::GetInstance().GetRenderer()->GetTexture("Assets/TitleLogo.png");
}


TitleUI::~TitleUI()
{
}

void TitleUI::Draw(Shader * shader) const
{
	DrawTexture(shader, mLogo);
}
