﻿#include "System.h"
#include "Input.h"
#include "Common.h"
#include "DrawComponentBase.h"
#include "Player.h"
#include "SceneBase.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "Camera.h"

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

		Input::GetInstance().LastUpdate();

		Draw();
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
		mCurrentScene = nullptr;
	}

	if (mRenderer != nullptr)
	{
		delete mRenderer;
		mRenderer = nullptr;
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

	for (auto sprCmp : mSpriteComponentList)
	{
		sprCmp->Draw();
	}

	mRenderer->Draw();

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

void System::ReportCameraDelete(const Camera * cam)
{
	if (mActiveCamera == cam)
	{
		mActiveCamera = nullptr;
	}
}
