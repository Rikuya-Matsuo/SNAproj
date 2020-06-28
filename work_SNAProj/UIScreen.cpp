#include "UIScreen.h"
#include "System.h"


UIScreen::UIScreen()
{
}


UIScreen::~UIScreen()
{
}

void UIScreen::Update()
{
}

void UIScreen::Draw(Shader * shader)
{
}

void UIScreen::ProcessInput(const uint8_t * keys)
{
	// ボタンがあるかの判定

	// マウスの位置を取得
	int x;
	int y;

	SDL_GetMouseState(&x, &y);

	// 画面の中央を(0,0)とする座標に変換する
	Vector2D mousePos(static_cast<float>(x), static_cast<float>(y));
	mousePos.x -= System::GetInstance().GetRenderer()->GetScreenWidth() / 2.0f;
	mousePos.y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2.0f - mousePos.y;

	// ボタンの強調

}

void UIScreen::HandleKeyPress(int key)
{
}

void UIScreen::Close()
{
}

void UIScreen::SetTitle(const std::string & text, const Vector3D & color, int pointSize)
{
}

void UIScreen::AddButton(const std::string & name, std::function<void()> onClick)
{
}

void UIScreen::DrawTexture(Shader * shader, Texture * texture, const Vector2D & offset, float scale)
{
}

void UIScreen::SetRelativeMouseMode(bool relativeFlag)
{
}
