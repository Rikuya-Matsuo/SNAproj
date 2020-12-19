#include "GameSceneTest.h"
#include "Player.h"
#include "Block.h"
#include "Stage.h"

GameSceneTest::GameSceneTest():
	GameScene("Map/TestMap0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png")
{
	// �w�i�̊R�𐶐�
	float blockSize = Block::mModelSize * mStage->GetBlockScale();
	mStage->LoadBGObjectMap("Map/TestMap0/Cliff.csv", blockSize / 2, mStage->GetFloorHeight(), -blockSize, blockSize, blockSize);
	Actor ** walls;
	//mStage->LoadBGObjectMap("Map/TestMap0/Wall.csv", )

	// �v���C���[�̐����E�ݒ�
	mPlayer = new Player;
	mActors.emplace_back(mPlayer);
	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	mPlayer->SetScale(25.0f);
	mPlayer->SetBeyondSceneFlag(true);

	// �J�����̃��[�h
	LoadCamera(mPlayer);

	// UI�̃��[�h
	LoadUI(mPlayer);
}


GameSceneTest::~GameSceneTest()
{
}
