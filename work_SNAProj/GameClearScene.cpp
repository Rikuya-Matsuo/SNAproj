#include "GameClearScene.h"
#include "GameClearBoard.h"
#include "Camera.h"
#include "TitleScene.h"
#include "Input.h"
#include "GuideUI.h"
#include "GameClearUI.h"

GameClearScene::GameClearScene()
{
	mUI = new GameClearUI();

	Camera * cam = new Camera();
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), Vector3D::zero, Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 20.0f, 0.0f));
	cam->SetActive();

	GuideUI * guideUI = new GuideUI("Assets/resultGuide.png");
}

GameClearScene::~GameClearScene()
{
	delete mUI;
}

void GameClearScene::Update()
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN) ||
		Input::GetInstance().GetAnyButtonPressedDown())
	{
		mNextScene = new TitleScene;

		mFlags |= mSceneChangeFlagMask;
	}
}
