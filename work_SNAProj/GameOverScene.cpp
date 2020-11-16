#include "GameOverScene.h"
#include "Input.h"
#include "TitleScene.h"
#include "Camera.h"
#include "GameOverUI.h"

GameOverScene::GameOverScene()
{
	// UIの生成
	mUI = new GameOverUI();

	// カメラの生成・設定
	Camera * cam = new Camera();
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), Vector3D::zero, Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetActive();
}

GameOverScene::~GameOverScene()
{
	delete mUI;
}

void GameOverScene::Update()
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
