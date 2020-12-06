#include "GameScene.h"
#include "Player.h"
#include "Camera.h"
#include "System.h"
#include "Input.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "Block.h"
#include "Stage.h"
#include "BGObject.h"
#include "EnemyWalker.h"
#include "EnemyManager.h"
#include "Input.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
#include "GameUI.h"

GameScene::GameScene()
{
	// ステージの生成
	mStage = new Stage;
	mStage->SetBlockScale(0.3f);
	mStage->LoadMap("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png");
	// 背景の崖を生成
	mStage->LoadBGObjectMap("Map/TestMap0/Cliff.csv", -100, mStage->GetFloorHeight(), -100, 0.f, 0.f);
	// 背景の壁を生成
	// 生成した壁のアクターを配列として取得し、モデルデータを回転させる
	Actor** walls;
	int wallMass = mStage->LoadBGObjectMap("Map/TestMap0/Wall.csv", 30, mStage->GetFloorHeight(), -75, 170, 0, &walls);
	if (wallMass != -1)
	{
		// 設定する角度
		const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));

		// 全ての壁を回転させる
		for (int i = 0; i < wallMass; ++i)
		{
			walls[i]->SetRotation(wallRota);
		}

		// ロード関数内で作られたActor*の配列のインスタンスを解放する
		delete[] walls;
	}

	// プレイヤーの生成・設定
	mPlayer = new Player;
	mActors.emplace_back(mPlayer);
	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	mPlayer->SetScale(25.0f);

	// エネミーのロード
	EnemyManager * em = new EnemyManager(mStage);
	em->LoadMapping("Map/TestMap0/enemyMapping.csv");

	// マネージャのインスタンスは不要なのでメモリを解放
	delete em;

	// カメラの生成
	Camera * cam = new Camera(mPlayer);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), mPlayer->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);
	cam->SetActive();

	// UIの生成
	mUI = new GameUI(mPlayer);

	// 光の設定
	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();
	dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	dir.mSpecColor = Vector3D(0.8f, 0.8f, 0.8f);
}

GameScene::~GameScene()
{
	delete mUI;
	delete mStage;
}

void GameScene::Update()
{
	// デバッグ用シーンジャンプの設定
	// -キーでジャンプ先をクリアシーンかゲームオーバーシーンかを切り替える
	// 左シフトキー + エンターキーでシーンジャンプを実行する
#ifdef DEBUG_SNA

	static bool clear = false;
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_MINUS))
	{
		clear = !clear;
		SDL_Log("Jump to clear : %s\n", clear ? "true" : "false");
	}

	bool lshift = Input::GetInstance().GetKey(SDL_SCANCODE_LSHIFT);
	bool enter = Input::GetInstance().GetKey(SDL_SCANCODE_RETURN);
	if (lshift && enter)
	{
		if (clear)
		{
			mNextScene = new GameClearScene;
		}
		else
		{
			mNextScene = new GameOverScene;
		}

		mFlags |= mSceneChangeFlagMask;
	}

#endif // DEBUG_SNA

	// 死亡時
	if (!mPlayer->GetAliveFlag())
	{
		// ゲームオーバーシーンにジャンプする
		mNextScene = new GameOverScene;
		mFlags |= mSceneChangeFlagMask;
	}

	// ゴール時
	if (mPlayer->GetPosition().x >= mStage->GetGoalLine())
	{
		// ゲームクリアシーンにジャンプする
		mNextScene = new GameClearScene;
		mFlags |= mSceneChangeFlagMask;
	}
}
