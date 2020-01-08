#include "System.h"
#include "Input.h"
#include "PhysicManager.h"
#include "Common.h"
#include "DrawComponentBase.h"
#include "Player.h"
#include "SceneBase.h"
#include "Renderer.h"
#include "Camera.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef DEBUG_SNA
Uint64 frameCount = 0;
#endif // DEBUG_SNA


System::System():
	mCurrentScene(nullptr),
	mRenderer(nullptr),
	mWindowWidth(1024),
	mWindowHeight(768),
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
	if (!mRenderer->Initialize(mWindowWidth, mWindowHeight, false))
	{
		SDL_Log("Failed to initialize the renderer.\n");
	}

	// サウンドの初期化
	if (!Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG))
	{
		delete mRenderer;
		return false;
	}
	SDL_Log("Success to Initialize SDLMixer\n");
	// SDLMixer API初期化　44100:音源の周波数 2:ステレオ 4096:内部バッファサイズ
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		delete mRenderer;
		Mix_CloseAudio();
		Mix_Quit();
		return false;
	}
	int decordNum = Mix_GetNumMusicDecoders();
	for (int i = 0; i < decordNum; ++i)
	{
		printf("MusicDecorder %d : %s\n", i, Mix_GetMusicDecoder(i));
	}
	int chunkNum = Mix_GetNumChunkDecoders();
	for (int i = 0; i < chunkNum; ++i)
	{
		printf("SoundDecorder %d : %s\n", i, Mix_GetChunkDecoder(i));
	}
	SDL_Log("Success to Initialize SDLMixerAPI\n");

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

		FixActorPosition();

		GravityFall();

		UpdateScene();

		UpdateActor();
		
		// 当たり判定
		PhysicManager::GetInstance().CheckHit();

		// カメラ更新
		if (mActiveCamera != nullptr)
		{
			mActiveCamera->Update();

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
		frameCount++;
#endif // DEBUG_SNA
	}

	SDL_Log("System quit running\n");
}

void System::Finish()
{
	Common::DeleteContainerOfPointer(mActorList);
	std::list<Actor *>().swap(mActorList);

	std::list<DrawComponentBase *>().swap(mSpriteComponentList);

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

void System::UpdateDeltaTime()
{
	Uint32 ticksCount = SDL_GetTicks();
	mDeltaTime = (ticksCount - mPrevTicksCount) / 1000.0f;
	mPrevTicksCount = ticksCount;

#ifdef SHOW_DELTA_TIME
	// 調整前のデルタタイム表示
	SDL_Log("%lf", mDeltaTime);
#endif

	// 30フレームのゲームにそぐわない値ならば調整する
	if (mDeltaTime > 1 / 30.0f)
	{
		mDeltaTime = 1 / 30.0f;
	}
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

void System::Draw()
{
	//SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
	glClearColor(0.2f, 0.6f, 0.9f, 1.0f);
	mRenderer->WindowClear();

	mRenderer->Draw();

	for (auto sprCmp : mSpriteComponentList)
	{
		sprCmp->Draw();
	}

	mRenderer->WindowFlip();
}

void System::ChangeScene(bool & quitFlag)
{
	SceneBase * nextScene = mCurrentScene->GetNextScene();
	if (nextScene != nullptr)
	{
		delete mCurrentScene;
		mCurrentScene = nextScene;

		auto itr = mActorList.begin();
		for (; itr != mActorList.end(); ++itr)
		{
			if (! (*itr)->GetBeyondSceneFlag())
			{
				delete *itr;

				itr--;
			}
			else
			{
				(*itr)->SetBeyondSceneFlag(false);
			}
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
/*
	int priority = actor->GetPriority();
	for (auto itr = mActorList.begin(); itr != mActorList.end(); ++itr)
	{
		if (priority < (*itr)->GetPriority())
		{
			mActorList.insert(itr, actor);
			return;
		}
	}
*/
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

void System::ResisterDrawComponent(const DrawComponentBase * in_cmp)
{
	const int drawOrder = in_cmp->GetDrawOrder();

	for (auto itr : mSpriteComponentList)
	{
		if (drawOrder < itr->GetDrawOrder())
		{
			auto insertPoint = std::find(mSpriteComponentList.begin(), mSpriteComponentList.end(), itr);
			mSpriteComponentList.insert(insertPoint, const_cast<DrawComponentBase*>(in_cmp));
			return;
		}
	}

	mSpriteComponentList.emplace_back(const_cast<DrawComponentBase*>(in_cmp));
}

void System::DeresisterDrawComponent(const DrawComponentBase * in_cmp)
{
	auto target = std::find(mSpriteComponentList.begin(), mSpriteComponentList.end(), in_cmp);

	if (target != mSpriteComponentList.end())
	{
		mSpriteComponentList.erase(target);
	}
}

void System::ReportCameraDelete(const Camera * cam)
{
	if (mActiveCamera == cam)
	{
		mActiveCamera = nullptr;
	}
}
