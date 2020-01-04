﻿#include "TestScene.h"
#include "Player.h"
#include "Camera.h"
#include "System.h"
#include "Input.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "AnimSpriteTest.h"
#include "SpriteTest.h"
#include "Block.h"
#include "TestStage.h"
#include "BGObject.h"



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
		ts->SetBlockScale(0.15f);
		ts->Load("");
	}
	else
	{
		AnimSpriteTest * ast = new AnimSpriteTest;
		ast->SetPosition(Vector3D(0.0f, 0.0f, 0.0f));
		ast->SetScale(30.0f);

		//SpriteTest * st = new SpriteTest;
		//st->SetPosition(Vector3D(-30.0f, 0.0f, 0.0f));
		//st->SetScale(30.0f);
	}

	// 壁オブジェクトのスケール値
	float wallScale = 0.5f;
	// 前に生成したオブジェクトの大きさを格納。どのくらい横にずらして生成すればいいかの変数である。
	float offset = 0.0f;
	// 岩壁の種類の順番をハードコーディング
	char wallType[] = { 1,2,1,3,3,2,1,3 };
	// ファイル名テンプレート
	const char * fileTemplate = "Assets/SM_IceCliffWall_%02da.gpmesh";
	for (int i = 0; i < 8; ++i)
	{
		const size_t fileNameMaxLen = 64;
		char fileName[fileNameMaxLen];
		sprintf_s(fileName, fileNameMaxLen, fileTemplate, wallType[i]);

		BGObject * bgWall = new BGObject(fileName);
		bgWall->SetScale(wallScale);
		bgWall->SetPosition(Vector3D(-100 + offset, -100, -50));
		offset = bgWall->GetModelSize().x * bgWall->GetScale() * i;

		if (i == 7)
		{
			printf("%lf\n", bgWall->GetModelSize().y);
		}
	}

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
