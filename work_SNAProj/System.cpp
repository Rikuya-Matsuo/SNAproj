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

System::System():
	mCurrentScene(nullptr),
	mRenderer(nullptr),
	mWindowWidth(1024),
	mWindowHeight(768)
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

		Input::GetInstance().Update();

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
	}

	SDL_Log("System quit running\n");
}

void System::Finish()
{
	Common::DeleteContainerOfPointer(mActorCollection);
	std::vector<Actor *>().swap(mActorCollection);

	std::list<DrawComponentBase *>().swap(mSpriteComponentList);

	if (mCurrentScene != nullptr)
	{
		delete mCurrentScene;
	}

	if (mRenderer != nullptr)
	{
		delete mRenderer;
	}

	IMG_Quit();
	SDL_Quit();

	SDL_Log("System finish to shut down");
}

void System::UpdateDeltaTime()
{
	Uint32 ticksCount = SDL_GetTicks();
	mDeltaTime = (ticksCount - mPrevTicksCount) / 1000.0f;
	mPrevTicksCount = ticksCount;
}

inline void System::UpdateScene()
{
	if (mCurrentScene != nullptr)
	{
		mCurrentScene->Update();
	}
}

void System::UpdateActor()
{
	for (auto actor : mActorCollection)
	{
		actor->Update();
	}
}

void System::Draw()
{
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
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

		for (int i = 0; i < (int)mActorCollection.size(); ++i)
		{
			if (!mActorCollection[i]->GetBeyondSceneFlag())
			{
				// アクターを削除しつつ添え字を一つ戻す
				delete mActorCollection[i--];
			}
			else
			{
				mActorCollection[i]->SetBeyondSceneFlag(false);
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
	for (auto actor : mActorCollection)
	{
		if (actor->GetAffectGravityFlag())
		{
			PhysicManager::GetInstance().GravityAffect(actor);
		}
	}
}

void System::ResisterActor(const Actor * in_act)
{
	mActorCollection.emplace_back(const_cast<Actor*>(in_act));
}

void System::DeresisterActor(const Actor * in_act)
{
	auto target = std::find(mActorCollection.begin(), mActorCollection.end(), const_cast<Actor*>(in_act));
	
	if (target != mActorCollection.end())
	{
		mActorCollection.erase(target);
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
