#include "System.h"
#include "Input.h"
#include "PhysicManager.h"
#include "Common.h"
#include "Player.h"
#include "SceneBase.h"
#include "Renderer.h"
#include "Camera.h"
#include "UIScreen.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef DEBUG_SNA
Uint64 System::mFrameCount = 0;
#endif // DEBUG_SNA

const float System::mMaxDeltaTime = 1.0f / 30.0f;

System::System():
	mCurrentScene(nullptr),
	mRenderer(nullptr),
	mWindowWidth(1920),
	mWindowHeight(1080),
	mSortActorFlag(false)
{
}

System::~System()
{
}

bool System::Init()
{
	SDL_Log("Start to initialize.\n");

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		SDL_Log("Failed to initialize SDL.\n--%s--\n", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Failed to initialize SDL_image.\n--%s--\n", SDL_GetError());
		return false;
	}

	mPrevTicksCount = SDL_GetTicks();

	SDL_Log("Success to initialize SDL\n");

	mRenderer = new Renderer;

	bool fullScreen;
#ifdef DEBUG_SNA
	fullScreen = false;
#else
	fullScreen = true;
#endif

	if (!mRenderer->Initialize(mWindowWidth, mWindowHeight, fullScreen))
	{
		SDL_Log("Failed to initialize the renderer.\n");
	}

	// 乱数種設定
	unsigned int seed = static_cast<unsigned int>(time(nullptr));
	srand(seed);

	// 本当ならここで何回か（５回ほど）rand()を空回しするとよいそうだが、
	// 乱数予測が重要なゲームではないことを鑑みて割愛。

	return true;
}

void System::Run()
{
	SDL_Log("System start running...\n");

	bool quitFlag = false;
	while (!quitFlag)
	{
		UpdateDeltaTime();

		if (mSortActorFlag)
		{
			SortActor();
			mSortActorFlag = false;
		}

		Input::GetInstance().Update();

#ifdef DEBUG_SNA
		if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_TAB))
		{
			// ブレークポイント設置
			SDL_Delay(0);
		}
#endif // DEBUG_SNA

		UpdateScene();

		UpdateActor();

		GravityFall();

		// 当たり判定
		PhysicManager::GetInstance().CheckHit();

		FixActorPosition();

		// UI更新
		UpdateTopUI();

		// カメラ更新
		if (mActiveCamera != nullptr)
		{
			mActiveCamera->Update();

			mRenderer->SetCamera(mActiveCamera);
			Matrix4 viewMatrix = mActiveCamera->GetViewMatrix();
			mRenderer->SetViewMatrix(viewMatrix);
		}

		// シーン遷移処理
		if (mCurrentScene->GetSceneChangeFlag())
		{
			ChangeScene(quitFlag);
		}

		// ゲーム終了処理
		if (Input::GetInstance().GetQuitEventFlag() || Input::GetInstance().GetKey(SDL_SCANCODE_ESCAPE))
		{
			quitFlag = true;
		}

		Draw();

		Input::GetInstance().LastUpdate();

#ifdef DEBUG_SNA
		mFrameCount++;
#endif // DEBUG_SNA
	}

	SDL_Log("System quit running\n");
}

void System::Finish()
{
	Common::DeleteContainerOfPointer(mActorList);
	std::list<Actor *>().swap(mActorList);

	if (mCurrentScene != nullptr)
	{
		delete mCurrentScene;
	}

	if (mRenderer != nullptr)
	{
		mRenderer->Shutdown();
		delete mRenderer;
	}

	IMG_Quit();
	SDL_Quit();

	SDL_Log("System finish to shut down");
}

//#define SHOW_DELTA_TIME
//#define SHOW_GAME_TIME

void System::UpdateDeltaTime()
{
	mDeltaTime = 0.0f;
	Uint32 ticksCount = SDL_GetTicks();
	while (mDeltaTime < mMaxDeltaTime)
	{
		ticksCount = SDL_GetTicks();
		mDeltaTime = (ticksCount - mPrevTicksCount) / 1000.0f;
	}
	mPrevTicksCount = ticksCount;
	mDeltaTime = mMaxDeltaTime;

#ifdef SHOW_DELTA_TIME
	// 調整前のデルタタイム表示
	SDL_Log("%lf", mDeltaTime);
#endif

	// 値が大きすぎれば調節する
	if (mDeltaTime > mMaxDeltaTime)
	{
		mDeltaTime = mMaxDeltaTime;
		SDL_Log("Delta time over\n");
	}
#ifdef SHOW_GAME_TIME
	static float timer = 0.0f;
	timer += mDeltaTime;
	SDL_Log("%lf", timer);
#endif
}

void System::SortActor()
{
	mActorList.sort([](const Actor * lhs, const Actor * rhs) { return lhs->GetPriority() <= rhs->GetPriority(); });
}

void System::FixActorPosition()
{
	for (auto actor : mActorList)
	{
		actor->FixPosition();
	}
}

void System::UpdateScene()
{
	if (mCurrentScene != nullptr)
	{
		mCurrentScene->Update();
	}
}

void System::UpdateActor()
{
	for (auto actor : mActorList)
	{
		actor->Update();
	}
}

void System::UpdateTopUI()
{
	if (!mActiveUIList.empty())
	{
		mActiveUIList.back()->Update();
	}
}

void System::Draw()
{
	glClearColor(0.2f, 0.6f, 0.9f, 1.0f);
	mRenderer->WindowClear();

	mRenderer->Draw();

	mRenderer->WindowFlip();
}

void System::ChangeScene(bool & quitFlag)
{
	PhysicManager::GetInstance().ClearHitState();

	SceneBase * nextScene = mCurrentScene->GetNextScene();
	if (nextScene != nullptr)
	{
		delete mCurrentScene;
		mCurrentScene = nextScene;

		auto itr = mActorList.begin();
		for (; itr != mActorList.end(); ++itr)
		{
			if ((*itr)->GetBelongScene() == mCurrentScene)
			{
				continue;
			}

			if (!(*itr)->GetBeyondSceneFlag())
			{
				delete *itr;

				itr--;
			}
			else
			{
				(*itr)->SetBeyondSceneFlag(false);
			}
		}

		for (itr = mActorList.begin(); itr != mActorList.end(); ++itr)
		{
			(*itr)->OnBeyondScene();
		}
	}
	else
	{
		quitFlag = true;
	}
}

void System::GravityFall()
{
	for (auto actor : mActorList)
	{
		if (actor->GetAffectGravityFlag())
		{
			PhysicManager::GetInstance().GravityAffect(actor);
		}
	}
}

void System::ResisterActor(const Actor * in_act)
{
	Actor * actor = const_cast<Actor*>(in_act);

	mActorList.emplace_back(const_cast<Actor*>(actor));
}

void System::DeresisterActor(const Actor * in_act)
{
	auto target = std::find(mActorList.begin(), mActorList.end(), const_cast<Actor*>(in_act));
	
	if (target != mActorList.end())
	{
		mActorList.erase(target);
	}
}

void System::ResisterUIScreen(const UIScreen * in_uiScr)
{
	mActiveUIList.emplace_back(const_cast<UIScreen*>(in_uiScr));
}

void System::DeresisterUIScreen(const UIScreen * in_uiScr)
{
	auto target = std::find(mActiveUIList.begin(), mActiveUIList.end(), const_cast<UIScreen*>(in_uiScr));

	if (target != mActiveUIList.end())
	{
		mActiveUIList.erase(target);
	}
}

void System::ReportCameraDelete(const Camera * cam)
{
	if (mActiveCamera == cam)
	{
		mActiveCamera = nullptr;
	}
}
