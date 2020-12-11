#include "GameSceneStage1.h"
#include "Block.h"
#include "Stage.h"

GameSceneStage1::GameSceneStage1():
	GameScene("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png")
{
	// ”wŒi‚ÌŠR‚ð¶¬
	float blockSize = Block::mModelSize * mStage->GetBlockScale();
	mStage->LoadBGObjectMap("Map/Map0/background.csv", blockSize / 2, mStage->GetFloorHeight(), -blockSize, blockSize, blockSize);
}

GameSceneStage1::~GameSceneStage1()
{
}
