#include "GameSceneStage2.h"
#include "Player.h"
#include "Stage.h"
#include "Block.h"

GameSceneStage2::GameSceneStage2():
	GameScene()
{
	// �X�e�[�W�̃��[�h
	LoadStage("Map/Map1/map.csv", "Assets/SM_Snow_Rock_Wall_A.png");

	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	//mPlayer->SetBeyondSceneFlag(true);

	// �w�i�̊R�𐶐�
	float blockSize = Block::mModelSize * mStage->GetBlockScale();
	mStage->LoadBGObjectMap("Map/Map1/background.csv", blockSize / 2, mStage->GetFloorHeight(), -blockSize, blockSize, blockSize);

	LoadCamera(mPlayer);

	LoadUI(mPlayer);
}

GameSceneStage2::~GameSceneStage2()
{
}
