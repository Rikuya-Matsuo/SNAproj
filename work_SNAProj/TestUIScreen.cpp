#include "TestUIScreen.h"
#include "System.h"
#include "Renderer.h"

TestUIScreen::TestUIScreen()
{
	mTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/Cube.png");
}

TestUIScreen::~TestUIScreen()
{
}

void TestUIScreen::Draw(Shader * shader) const
{
	DrawTexture(shader, mTexture);
}
