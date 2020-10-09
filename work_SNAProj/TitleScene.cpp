#include "TitleScene.h"
#include "TitleBoard.h"
#include "Camera.h"
#include "Input.h"
#include "GameScene.h"
#include "Common.h"
#include "TitleUI.h"
#include "TitleCameraTargetActor.h"
#include "Stage.h"

TitleScene::TitleScene():
	SceneBase(),
	mUI(new TitleUI)
{
	mStage = new Stage();
	mStage->SetBlockScale(0.3f);
	mStage->LoadMap("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png");
	mStage->LoadBGObjectMap("Map/Map0/Cliff.csv", -100, mStage->GetFloorHeight(), -100, 0.f, 0.f);
	Actor** walls;
	int wallMass = mStage->LoadBGObjectMap("Map/Map0/Wall.csv", 30, mStage->GetFloorHeight(), -75, 170, 0, &walls);

	if (wallMass != -1)
	{
		// 背景の壁の回転
		const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));

		for (int i = 0; i < wallMass; ++i)
		{
			walls[i]->SetRotation(wallRota);
		}

		delete[] walls;
	}

	TitleCameraTargetActor* tcta = new TitleCameraTargetActor();
	tcta->SetPosition(Vector3D(30.0f, 0.0f, 0.0f));

	Camera * cam = new Camera(tcta);
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), tcta->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 80.0f, 130.0f));
	cam->SetActive();
}

TitleScene::~TitleScene()
{
	delete mUI;

	delete mStage;

	while (!mCameras.empty())
	{
		delete mCameras.back();
		mCameras.pop_back();
	}
}

void TitleScene::Update()
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN) ||
		Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_A))
	{
		mNextScene = new GameScene;

		mFlags |= mSceneChangeFlagMask;
	}
}
