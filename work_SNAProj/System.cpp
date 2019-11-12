#include "System.h"
#include "Input.h"
#include "Common.h"
#include "DrawComponentBase.h"
#include "Player.h"

System::System()
{
}

System::~System()
{
}

bool System::Init()
{
	SDL_Log("Start to initialize.\n");

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		SDL_Log("Failed to initialize SDL.\n--%s--\n", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Test", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create a window.\n--%s--\n", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer.\n--%s--\n", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Failed to initialize SDL_image.\n--%s--\n", SDL_GetError());
		return false;
	}

	mPrevTicksCount = SDL_GetTicks();

	SDL_Log("Success to initialize SDL\n");

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

		if (Input::GetInstance().GetQuitEventFlag() || Input::GetInstance().GetState(SDL_SCANCODE_ESCAPE))
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
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	SDL_Log("System finish to shut down");
}

void System::UpdateDeltaTime()
{
	Uint32 ticksCount = SDL_GetTicks();
	mDeltaTime = (ticksCount - mPrevTicksCount) / 1000.0f;
	mPrevTicksCount = ticksCount;
}

void System::Draw()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	for (auto drawCmp : mDrawComponentList)
	{
		drawCmp->Draw();
	}

	SDL_RenderPresent(mRenderer);
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
	for (auto itr : mDrawComponentList)
	{
		if (drawOrder < itr->GetDrawOrder())
		{
			auto insertPoint = std::find(mDrawComponentList.begin(), mDrawComponentList.end(), itr);
			mDrawComponentList.insert(insertPoint, const_cast<DrawComponentBase*>(in_cmp));
			inserted = true;
			break;
		}
	}

	if (!inserted)
	{
		mDrawComponentList.emplace_back(const_cast<DrawComponentBase*>(in_cmp));
	}
}

void System::DeresisterDrawComponent(const DrawComponentBase * in_cmp)
{
	auto target = std::find(mDrawComponentList.begin(), mDrawComponentList.end(), in_cmp);

	if (target != mDrawComponentList.end())
	{
		mDrawComponentList.erase(target);
	}
}
