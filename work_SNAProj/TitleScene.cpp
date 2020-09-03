#include "TitleScene.h"
#include "TitleBoard.h"
#include "Camera.h"
#include "Input.h"
#include "GameScene.h"
#include "GuideUI.h"
#include "Common.h"
#include "TitleUI.h"

TitleScene::TitleScene()
{
	mUI = new TitleUI;

	Camera * cam = new Camera();
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), Vector3D::zero, Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 55.0f, 0.0f));
	cam->SetActive();
	//cam->SetWatchTargetFlag(false);
	//cam->SetChaseTargetFlag(false);

	GuideUI * guideUI = new GuideUI("Assets/titleGuide.png");
	guideUI->SetRotationAngle(Common::DegToRad(0.0f));
	Vector3D pos = guideUI->GetPosition();
	pos.z += 578.f;
	guideUI->SetPosition(pos);
	float scale = guideUI->GetScale();
	guideUI->SetScale(scale / 9);
}

TitleScene::~TitleScene()
{
	delete mUI;

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
