#include "GameClearScene.h"
#include "GameClearBoard.h"
#include "Camera.h"
#include "TitleScene.h"
#include "Input.h"
#include "GuideUI.h"

GameClearScene::GameClearScene()
{
	GameClearBoard * gcb = new GameClearBoard;
	gcb->SetScale(80.0f);
	gcb->SetPosition(Vector3D::zero);

	Camera * cam = new Camera(gcb);
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), gcb->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 20.0f, 0.0f));
	cam->SetActive();

	GuideUI * guideUI = new GuideUI("Assets/resultGuide.png");
}

GameClearScene::~GameClearScene()
{
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
