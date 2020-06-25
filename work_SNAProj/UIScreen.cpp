#include "UIScreen.h"



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
