#include "GameScene.h"
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
#include <thread>

GameScene::GameScene():
	mPrevNowLoadingFlag(false)
{
	SceneBase::mNowLoadingFlag = true;

	std::thread th(Load, this);

	th.swap(mLoadingThread);
}

GameScene::~GameScene()
{
	delete mUI;
	delete mStage;
}

void GameScene::Update()
{
	// ロード中はゲームを動かさない
	if (SceneBase::mNowLoadingFlag)
	{
		mPrevNowLoadingFlag = SceneBase::mNowLoadingFlag;
		return;
	}

	// ロード完了後初めてのアップデートならばスレッドをジョイン
	if (mPrevNowLoadingFlag)
	{
		mLoadingThread.join();
		mPrevNowLoadingFlag = SceneBase::mNowLoadingFlag;
	}

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

void GameScene::Load(GameScene * inputThisPtr)
{
	SDL_Window * mainWindow = SDL_GL_GetCurrentWindow();
	SDL_GLContext mainContext = SDL_GL_GetCurrentContext();

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

	SDL_Window * window = SDL_CreateWindow("Loading Thread's window", 0, 0, 0, 0,
		SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	//SDL_GL_MakeCurrent(window, mainContext);

	inputThisPtr->mStage = new Stage;
	inputThisPtr->mStage->SetBlockScale(0.3f);
	inputThisPtr->mStage->LoadMap("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png");
	inputThisPtr->mStage->LoadBGObjectMap("Map/Map0/Cliff.csv", -100, inputThisPtr->mStage->GetFloorHeight(), -100, 0.f, 0.f);
	Actor** walls;
	int wallMass = inputThisPtr->mStage->LoadBGObjectMap("Map/Map0/Wall.csv", 30, inputThisPtr->mStage->GetFloorHeight(), -75, 170, 0, &walls);
	if (wallMass != -1)
	{
		const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));

		for (int i = 0; i < wallMass; ++i)
		{
			walls[i]->SetRotation(wallRota);
		}

		delete[] walls;
	}

	inputThisPtr->mPlayer = new Player;
	inputThisPtr->mActors.emplace_back(inputThisPtr->mPlayer);
	inputThisPtr->mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	inputThisPtr->mPlayer->SetScale(25.0f);

	const bool genEnemy = true;
	if (genEnemy)
	{
		EnemyManager * em = new EnemyManager(inputThisPtr->mStage);
		em->LoadMapping("Map/Map0/enemyMapping.csv");

		delete em;
	}

	Camera * cam = new Camera(inputThisPtr->mPlayer);
	inputThisPtr->mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), inputThisPtr->mPlayer->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);
	cam->SetActive();

	inputThisPtr->mUI = new GameUI(inputThisPtr->mPlayer);

	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();
	dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	dir.mSpecColor = Vector3D(0.8f, 0.8f, 0.8f);

	SDL_GL_MakeCurrent(mainWindow, mainContext);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SceneBase::mNowLoadingFlag = false;
}
