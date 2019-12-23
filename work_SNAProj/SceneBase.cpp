#include "SceneBase.h"
#include "Actor.h"
#include "Camera.h"

const SceneBase::FlagType SceneBase::mSceneChangeFlagMask = 1 << 0;

SceneBase::SceneBase():
	mNextScene(nullptr),
	mFlags(0)
{
	mCameras.reserve(1);
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
