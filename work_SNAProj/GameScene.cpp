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

Player * GameScene::mPlayer = nullptr;

GameScene::GameScene():
	mGoalFlag(false)
{
	// ステージの生成
	mStage = new Stage;
	float blockScale = 0.3f;
	mStage->SetBlockScale(blockScale);

	// 光の設定
	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();
	dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	dir.mSpecColor = Vector3D(0.8f, 0.8f, 0.8f);
}

GameScene::~GameScene()
{
	delete mUI;

	if (mStage)
	{
		delete mStage;
	}
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

		// プレイヤーのシーン跨ぎフラグは忘れずに切っておく
		mPlayer->SetBeyondSceneFlag(false);

		mFlags |= mSceneChangeFlagMask;
	}

	// ゴール時
	// 条件は、メンバーのゴールフラグが真 or ゴールブロックが設置されていないマップでプレイヤーがゴールラインを超えていること
	bool goal = mGoalFlag ||
		(!mStage->GetGoalBlockFlag() && mPlayer->GetPosition().x >= mStage->GetGoalLine());
	if (goal)
	{
		OnGoal();
	}
}

void GameScene::OnGoal()
{
	// ゲームクリアシーンにジャンプする
	mNextScene = new GameClearScene;
	mFlags |= mSceneChangeFlagMask;

	mPlayer = nullptr;
}

void GameScene::LoadCamera(Player* player)
{
	// カメラの生成
	Camera* cam = new Camera(player);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), player->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);
	cam->SetActive();
}

void GameScene::LoadUI(Player * player)
{
	// UIの生成
	mUI = new GameUI(mPlayer);
}

void GameScene::LoadEnemy(const std::string & enemyMappingDataPath)
{
	// エネミーのロード
	EnemyManager * em = new EnemyManager(mStage);
	em->LoadMapping(enemyMappingDataPath);

	// マネージャのインスタンスは不要なのでメモリを解放
	delete em;
}
