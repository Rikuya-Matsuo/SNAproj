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
	DrawTexture(shader, mTexture, Vector2D(0.0f, 0.0f), 100.0f);
}
