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
	// �{�^�������邩�̔���

	// �}�E�X�̈ʒu���擾
	int x;
	int y;

	SDL_GetMouseState(&x, &y);

	// ��ʂ̒�����(0,0)�Ƃ�����W�ɕϊ�����
	Vector2D mousePos(static_cast<float>(x), static_cast<float>(y));
	mousePos.x -= System::GetInstance().GetRenderer()->GetScreenWidth() / 2.0f;
	mousePos.y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2.0f - mousePos.y;

	// �{�^���̋���

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
