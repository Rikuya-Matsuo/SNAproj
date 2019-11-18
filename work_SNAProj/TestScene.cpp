#include "TestScene.h"
#include "Player.h"

TestScene::TestScene()
{
	Player * p = new Player;
	mActors.emplace_back(p);
	p->SetPosition(Vector3D(0.0f, 0.0f));
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
}
