#include "TestScene.h"
#include "Player.h"
#include "SpriteTest.h"
#include "Camera.h"
#include "System.h"
#include "Renderer.h"

TestScene::TestScene()
{
	Player * p = new Player;
	mActors.emplace_back(p);
	p->SetPosition(Vector3D(0.0f, 0.0f, 0.0f));

	SpriteTest * st = new SpriteTest;
	st->SetPosition(Vector3D(0.0f, 0.0f));

	Camera * cam = new Camera(p);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), p->GetPosition(), Vector3D(0, 0, 1));

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
