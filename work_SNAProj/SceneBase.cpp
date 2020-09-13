#include "SceneBase.h"
#include "Actor.h"
#include "Camera.h"
#include "LoadingTimeUI.h"

const SceneBase::FlagType SceneBase::mSceneChangeFlagMask = 1 << 0;

SceneBase * SceneBase::mLatestScene = nullptr;

UIScreen * SceneBase::mLoadingTimeUI = nullptr;

SceneBase::SceneBase():
	mNextScene(nullptr),
	mStage(nullptr),
	mFlags(0)
{
	mCameras.reserve(1);

	mLatestScene = this;

	if (mLoadingTimeUI == nullptr)
	{
		mLoadingTimeUI = new LoadingTimeUI;
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

void SceneBase::OnEndGame()
{
	delete mLoadingTimeUI;
}
