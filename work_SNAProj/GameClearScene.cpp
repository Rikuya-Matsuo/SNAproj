#include "GameClearScene.h"
#include "Camera.h"
#include "TitleScene.h"
#include "Input.h"
#include "GameClearUI.h"

GameClearScene::GameClearScene()
{
	// UIの生成
	mUI = new GameClearUI();

	// カメラの生成・設定
	Camera * cam = new Camera();
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), Vector3D::zero, Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetActive();
}

GameClearScene::~GameClearScene()
{
	delete mUI;
}

void GameClearScene::Update()
{
	// Enterキーかゲームパッドのいずれかのボタンでタイトルシーンへ
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN) ||
		Input::GetInstance().GetAnyButtonPressedDown())
	{
		// 次のシーンの設定
		mNextScene = new TitleScene;

		// シーン変更フラグを真に
		mFlags |= mSceneChangeFlagMask;
	}
}
