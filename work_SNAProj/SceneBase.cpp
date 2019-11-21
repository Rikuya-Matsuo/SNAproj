#include "SceneBase.h"
#include "Actor.h"

SceneBase::SceneBase()
{
	mCameras.reserve(1);
}

SceneBase::~SceneBase()
{
	for (auto actor : mActors)
	{
		delete actor;
	}
	mActors.clear();

	std::vector<Actor *>().swap(mActors);
}

void SceneBase::Draw()
{
}
