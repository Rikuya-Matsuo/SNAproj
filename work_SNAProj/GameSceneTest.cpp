#include "GameSceneTest.h"
#include "Player.h"
#include "Block.h"
#include "Stage.h"

GameSceneTest::GameSceneTest():
	GameScene()
{
	// �X�e�[�W�̃��[�h
	LoadStage("Map/TestMap0/map.csv", "Assets/SM_Snow_Rock_Wall_A.png");

	// �w�i�̊R�𐶐�
	float blockSize = Block::mModelSize * mStage->GetBlockScale();
	mStage->LoadBGObjectMap("Map/TestMap0/Cliff.csv", -100, mStage->GetFloorHeight(), -100, 0.f, 0.f);
	Actor ** walls;
	int wallMass = mStage->LoadBGObjectMap("Map/TestMap0/Wall.csv", 30, mStage->GetFloorHeight(), -75, 170, 0, &walls);
	if (wallMass != -1)
	{
		const Quaternion wallRota = Quaternion(Vector3D(0, 0, 1), static_cast<float>(M_PI / 2.0f));

		// �S�Ă̕ǂ���]������
		for (int i = 0; i < wallMass; ++i)
		{
			walls[i]->SetRotation(wallRota);
		}

		// ���[�h�֐����ō��ꂽActor*�̔z��̃C���X�^���X���������
		delete[] walls;
	}

	// �v���C���[�̐����E�ݒ�
	mPlayer = new Player;
	mActors.emplace_back(mPlayer);
	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	mPlayer->SetScale(25.0f);

	LoadEnemy("Map/TestMap0/enemyMapping.csv");

	// �J�����̃��[�h
	LoadCamera(mPlayer);

	// UI�̃��[�h
	LoadUI(mPlayer);
}

GameSceneTest::~GameSceneTest()
{
}
