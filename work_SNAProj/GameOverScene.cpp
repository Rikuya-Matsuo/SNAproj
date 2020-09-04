#include "GameOverScene.h"
#include "Input.h"
#include "TitleScene.h"
#include "Camera.h"
#include "GuideUI.h"
#include "GameOverUI.h"

GameOverScene::GameOverScene()
{
	mUI = new GameOverUI();

	Camera * cam = new Camera();
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), Vector3D::zero, Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetActive();

	GuideUI * guideUI = new GuideUI("Assets/resultGuide.png");
}

GameOverScene::~GameOverScene()
{
	delete mUI;
}

void GameOverScene::Update()
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN) ||
		Input::GetInstance().GetAnyButtonPressedDown())
	{
		mNextScene = new TitleScene;

		mFlags |= mSceneChangeFlagMask;
	}
}
