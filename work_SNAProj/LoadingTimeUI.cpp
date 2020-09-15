#include "LoadingTimeUI.h"
#include "System.h"
#include "AnimationChips.h"
#include "SceneBase.h"

LoadingTimeUI::LoadingTimeUI()
{
	mBackGround = System::GetInstance().GetRenderer()->GetTexture("Assets/black1dot.png");

	mAnimation = new AnimationChips;
	mAnimation->Load(System::GetInstance().GetRenderer(), "Assets/NinjaRun02.png", 5, 5, 1, 128, 128, 0.05f);
	int routine[] = { 0, 1, 2, 3, -1 };
	mAnimation->SetRoutine(routine);

	mTextTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/LoadingText.png");
}

LoadingTimeUI::~LoadingTimeUI()
{
	delete mAnimation;
}

void LoadingTimeUI::Update()
{
	UIScreen::Update();

	mAnimation->Update();
}

void LoadingTimeUI::Draw(Shader * shader) const
{
	if (!SceneBase::GetNowLoadingFlag())
	{
		return;
	}

	DrawTexture(shader, mBackGround, Vector2D::zero, 2000.0f);

	float scale = 2.0f;
	Texture * tex = mAnimation->GetCurrentTexture();
	DrawTexture(shader, tex, Vector2D(0.0f, 50.f), scale);

	DrawTexture(shader, mTextTexture, Vector2D(0.0f, -300.f));
}
