#include "TestScene.h"
#include "Player.h"
#include "SpriteTest.h"
#include "Block.h"
#include "Camera.h"
#include "System.h"
#include "Renderer.h"
#include "Quaternion.h"

#include <cmath>

TestScene::TestScene()
{
	Player * p = new Player;
	mActors.emplace_back(p);
	p->SetPosition(Vector3D(-30.0f, 0.0f, 0.0f));

	Block * bk = new Block;
	bk->SetPosition(Vector3D(30.0f, 0.0f, 0.0f));

	SpriteTest * st = new SpriteTest;
	st->SetPosition(Vector3D(0.0f, 0.0f));
	st->SetScale(100.0f);
	Quaternion q(Vector3D(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI / 2.0f));
	st->SetRotation(q);

	Camera * cam = new Camera(p);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), st->GetPosition(), Vector3D(0, 0, 1));

	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();
	dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	dir.mSpecColor = Vector3D(0.8f, 0.8f, 0.8f);
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
}
