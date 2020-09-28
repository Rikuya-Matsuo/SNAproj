#include "SceneBase.h"
#include "Actor.h"
#include "Camera.h"
#include "SDLScreen.h"
#include "System.h"

const SceneBase::FlagType SceneBase::mSceneChangeFlagMask = 1 << 0;

SceneBase * SceneBase::mLatestScene = nullptr;

SDLScreen * SceneBase::mLoadingScreen = nullptr;

bool SceneBase::mNowLoadingFlag = false;

std::thread SceneBase::mLoadingScreenThread;

SceneBase::SceneBase():
	mNextScene(nullptr),
	mStage(nullptr),
	mFlags(0)
{
	mCameras.reserve(1);

	mLatestScene = this;

	// もし、ロード画面がまだ生成されてなければ、生成する
	if (!mLoadingScreen)
	{
		mLoadingScreen = new SDLScreen;
	}
}

SceneBase::~SceneBase()
{
	// カメラの削除
	while (!mCameras.empty())
	{
		delete mCameras.back();
		mCameras.pop_back();
	}

	// ベクターコンテナのメモリ開放
	mActors.shrink_to_fit();
	mCameras.shrink_to_fit();

	std::vector<Actor *>().swap(mActors);
	std::vector<Camera *>().swap(mCameras);
}

void SceneBase::Draw()
{
}

void SceneBase::PlayLoadingScreen(SDL_Renderer * renderer)
{
	const Renderer * mainRenderer = System::GetInstance().GetRenderer();

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 400;
	rect.y = 100;

	int velocity = 20;

	const int wLow = 100;
	const int wHigh = 1000;

	char count = 0;
	const char countEnd = 60;

	while (count < countEnd)
	{
		if (rect.w >= wHigh || rect.w <= wLow)
		{
			velocity *= -1;
		}
		rect.w += velocity;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);

		SDL_RenderPresent(renderer);

		count += !mNowLoadingFlag;
	}
}
