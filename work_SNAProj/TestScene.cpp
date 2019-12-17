#include "TestScene.h"
#include "Player.h"
#include "Camera.h"
#include "System.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "AnimSpriteTest.h"
#include "SpriteTest.h"
#include "Block.h"
#include "TestStage.h"

#include <cmath>

TestScene::TestScene()
{
	Player * p = new Player;
	mActors.emplace_back(p);
	p->SetPosition(Vector3D(0.0f, 0.0f, 50.0f));
	p->SetScale(25.0f);

	const bool stageGen = true;
	
	if (stageGen)
	{
		TestStage * ts = new TestStage;
		ts->SetBlockScale(0.1f);
		ts->Load("");
	}
	else
	{
		SpriteTest * st = new SpriteTest;
		st->SetPosition(Vector3D(-30.0f, 0.0f, 0.0f));
		st->SetScale(30.0f);
	}

	//AnimSpriteTest * ast = new AnimSpriteTest;
	//ast->SetPosition(Vector3D(0.0f, 0.0f, 0.0f));
	//ast->SetScale(30.0f);

	Camera * cam = new Camera(p);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), p->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);

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
