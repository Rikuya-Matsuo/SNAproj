#include "TestScene.h"
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
#include "EnemyTest.h"


TestScene::TestScene()
{
	Player * p = new Player;
	mActors.emplace_back(p);
	p->SetPosition(Vector3D(0.0f, 0.0f, 50.0f));
	p->SetScale(25.0f);

	TestStage * ts = new TestStage;
	ts->SetBlockScale(0.3f);
	ts->Load("");

	GenerateBGWall(ts->GetFloorHeight());

	GenerateBGDecoration(ts->GetFloorHeight());

	EnemyTest * et = new EnemyTest;
	et->SetScale(25.0f);
	et->SetPosition(Vector3D(50.0f, 0.0f, 100.0f));

	Camera * cam = new Camera(p);
	mCameras.emplace_back(cam);
	cam->Init(Vector3D(0, 100, 100), p->GetPosition(), Vector3D(0, 0, 1));
	cam->SetDistanceVector(Vector3D(0, 150, 50));
	cam->SetChaseTargetFlag(true);
	cam->SetActive();

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

void TestScene::GenerateBGWall(float height)
{
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
		bgWall->SetPosition(Vector3D(-100 + offset, -100, height));
		offset = bgWall->GetModelSize().x * bgWall->GetScale() * i;
	}
}

void TestScene::GenerateBGDecoration(float height)
{
	// キャプチャの意味をここで知ったのだ……
	// []の中に変数を入れると、ラムダ式の中でそれが使えるらしい。
	auto gen = [height](const std::string & modelPath, float x, float y)
	{
		BGObject * bgWall = new BGObject(modelPath);
		bgWall->SetPosition(Vector3D(x, y, height));

		return bgWall;
	};

	const float defaultDepth = -75;
	const std::string assets = "Assets/";

	const float wallScale = 0.2f;
	const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));
	auto wallSetting = [wallScale, &wallRota](BGObject * obj)
	{
		obj->SetScale(wallScale);
		obj->SetRotation(wallRota);
	};

	BGObject * generated = nullptr;

	generated = gen(assets + "SM_RuinedWalls1.gpmesh", 30, defaultDepth);
	wallSetting(generated);

	generated = gen(assets + "SM_RuinedWalls2.gpmesh", 200, defaultDepth);
	wallSetting(generated);
}
