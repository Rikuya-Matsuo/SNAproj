﻿#include "GameScene.h"
#include "Player.h"
#include "Camera.h"
#include "System.h"
#include "Input.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "AnimSpriteTest.h"
#include "SpriteTest.h"
#include "Block.h"
#include "Stage.h"
#include "BGObject.h"
#include "EnemyTest.h"
#include "EnemyManager.h"
#include "Input.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
#include "LifeUIManager.h"
#include "GuideUI.h"
#include "GameUI.h"

GameScene::GameScene()
{
	mStage = new Stage;
	mStage->SetBlockScale(0.3f);
	mStage->LoadMap("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png");
	mStage->LoadBGObjectMap("Map/Map0/Cliff.csv", -100, mStage->GetFloorHeight(), -100);

	mPlayer = new Player;
	mActors.emplace_back(mPlayer);
	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	mPlayer->SetScale(25.0f);

	const bool genEnemy = true;
	if (genEnemy)
	{
		EnemyManager * em = new EnemyManager(mStage);
		em->LoadMapping("Map/Map0/enemyMapping.csv");

		delete em;
	}

	GenerateBGDecoration(mStage->GetFloorHeight());

	Camera * cam = new Camera(mPlayer);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), mPlayer->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);
	cam->SetActive();

	mUI = new GameUI(mPlayer);

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

	mUI->Update();

	//mLifeUI->Update();

	if (!mPlayer->GetAliveFlag())
	{
		mNextScene = new GameOverScene;
		mFlags |= mSceneChangeFlagMask;
	}

	if (mPlayer->GetPosition().x >= mStage->GetGoalLine())
	{
		mNextScene = new GameClearScene;
		mFlags |= mSceneChangeFlagMask;
	}
}

void GameScene::GenerateBGWall(float height)
{
	// 壁オブジェクトのスケール値
	float wallScale = 0.5f;
	// 前に生成したオブジェクトの大きさを格納。どのくらい横にずらして生成すればいいかの変数である。
	float offset = 0.0f;
	// 岩壁の種類の順番をハードコーディング
	char wallType[] = { 1,2,1,3,3,2,1,3 };
	// ファイル名テンプレート
	const char * fileTemplate = "Assets/SM_IceCliffWall_%02da.gpmesh";
	for (int i = 0; i < 8; ++i)
	{
		const size_t fileNameMaxLen = 64;
		char fileName[fileNameMaxLen];
		sprintf_s(fileName, fileNameMaxLen, fileTemplate, wallType[i]);

		BGObject * bgWall = new BGObject(fileName);
		bgWall->SetScale(wallScale);
		bgWall->SetPosition(Vector3D(-100 + offset, -100, height));
		offset = bgWall->GetModelSize().x * bgWall->GetScale() * i;
	}
}

void GameScene::GenerateBGDecoration(float height)
{
	// キャプチャの意味をここで知ったのだ……
	// []の中に変数を入れると、ラムダ式の中でそれが使えるらしい。
	auto gen = [height](const std::string & modelPath, float x, float y)
	{
		BGObject * bgWall = new BGObject(modelPath);
		bgWall->SetPosition(Vector3D(x, y, height));

		return bgWall;
	};

	const float defaultDepth = -75;
	const std::string assets = "Assets/";

	const float wallScale = 0.2f;
	const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));
	auto wallSetting = [wallScale, &wallRota](BGObject * obj)
	{
		obj->SetScale(wallScale);
		obj->SetRotation(wallRota);
	};

	BGObject * generated = nullptr;

	generated = gen(assets + "SM_RuinedWalls1.gpmesh", 30, defaultDepth);
	wallSetting(generated);

	generated = gen(assets + "SM_RuinedWalls2.gpmesh", 200, defaultDepth);
	wallSetting(generated);
}
