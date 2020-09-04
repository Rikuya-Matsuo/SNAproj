#include "TestUIScreen.h"
#include "System.h"
#include "Renderer.h"

TestUIScreen::TestUIScreen()
{
	mTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/TitleLogo.png");
}

TestUIScreen::~TestUIScreen()
{
}

void TestUIScreen::Draw(Shader * shader) const
{
	Vector2D pos(0.0f, 0.0f);
	DrawTexture(shader, mTexture, pos, 1.0f);
}
