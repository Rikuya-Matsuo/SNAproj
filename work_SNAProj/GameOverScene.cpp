#include "GameOverScene.h"
#include "Input.h"
#include "TitleScene.h"
#include "GameOverBoard.h"
#include "Camera.h"

GameOverScene::GameOverScene()
{
	GameOverBoard * gob = new GameOverBoard;
	gob->SetScale(80.0f);
	gob->SetPosition(Vector3D::zero);

	Camera * cam = new Camera(gob);
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), gob->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 55.0f, 0.0f));
	cam->SetActive();
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Update()
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN))
	{
		mNextScene = new TitleScene;

		mFlags |= mSceneChangeFlagMask;
	}
}
