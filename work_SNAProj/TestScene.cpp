#include "TestScene.h"
#include "Camera.h"
#include "System.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "AnimSpriteTest.h"
#include "SpriteTest.h"
#include "Block.h"

#include <cmath>

TestScene::TestScene()
{
	//Player * p = new Player;
	//mActors.emplace_back(p);
	//p->SetPosition(Vector3D(-30.0f, 0.0f, 0.0f));

	Block * bk = new Block;
	bk->SetPosition(Vector3D(30.0f, 0.0f, 0.0f));
	bk->SetScale(0.1f);

	SpriteTest * st = new SpriteTest;
	st->SetPosition(Vector3D(-30.0f, 0.0f, 0.0f));
	st->SetScale(30.0f);

	AnimSpriteTest * ast = new AnimSpriteTest();
	ast->SetPosition(Vector3D(0.0f, 0.0f, 0.0f));
	ast->SetScale(30.0f);

	Camera * cam = new Camera(st);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), st->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 300, 50));

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
