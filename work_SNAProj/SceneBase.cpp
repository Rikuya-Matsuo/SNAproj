#include "SceneBase.h"
#include "Actor.h"

SceneBase::SceneBase()
{
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
