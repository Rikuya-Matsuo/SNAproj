#include "System.h"
#include "Input.h"
#include "Common.h"
#include "DrawComponentBase.h"
#include "Player.h"
#include "SceneBase.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "Camera.h"

void System::ReportCameraDelete(const Camera * cam)
{
	if (mActiveCamera == cam)
	{
		mActiveCamera = nullptr;
	}
}

System::System():
	//mWindow(nullptr),
	//mSDLRenderer(nullptr),
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
	/*
	mWindow = SDL_CreateWindow("Test", 100, 100, mWindowWidth, mWindowHeight, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create a window.\n--%s--\n", SDL_GetError());
		return false;
	}

	mSDLRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mSDLRenderer)
	{
		SDL_Log("Failed to create renderer.\n--%s--\n", SDL_GetError());
		return false;
	}
	*/
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

		UpdateScene();

		UpdateActor();
		//ComponentManager::GetInstance().UpdateComponents();

		if (mActiveCamera != nullptr)
		{
			Matrix4 viewMatrix = mActiveCamera->GetViewMatrix();
			mRenderer->SetViewMatrix(viewMatrix);
		}


		if (Input::GetInstance().GetQuitEventFlag() || Input::GetInstance().GetKey(SDL_SCANCODE_ESCAPE))
		{
			quitFlag = true;
		}

		Input::GetInstance().LastUpdate();

		Draw();
	}

	SDL_Log("System quit running\n");
}

void System::Finish()
{
	Common::DeleteContainerOfPointer(mActorCollection);

	IMG_Quit();
	//SDL_DestroyRenderer(mSDLRenderer);
	//SDL_DestroyWindow(mWindow);
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

	for (auto sprCmp : mSpriteComponentList)
	{
		sprCmp->Draw();
	}

	mRenderer->Draw();

	mRenderer->WindowFlip();
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

	bool inserted = false;
	for (auto itr : mSpriteComponentList)
	{
		if (drawOrder < itr->GetDrawOrder())
		{
			auto insertPoint = std::find(mSpriteComponentList.begin(), mSpriteComponentList.end(), itr);
			mSpriteComponentList.insert(insertPoint, const_cast<DrawComponentBase*>(in_cmp));
			inserted = true;
			break;
		}
	}

	if (!inserted)
	{
		mSpriteComponentList.emplace_back(const_cast<DrawComponentBase*>(in_cmp));
	}
}

void System::DeresisterDrawComponent(const DrawComponentBase * in_cmp)
{
	auto target = std::find(mSpriteComponentList.begin(), mSpriteComponentList.end(), in_cmp);

	if (target != mSpriteComponentList.end())
	{
		mSpriteComponentList.erase(target);
	}
}
