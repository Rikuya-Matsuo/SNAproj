#include "GameSceneStage2.h"
#include "Player.h"

GameSceneStage2::GameSceneStage2():
	GameScene("Map/Map0/map.csv", "Assets/SM_Ice_RuinedWalls.png", "Assets/SM_Snow_Rock_Wall_A.png")
{
	mPlayer->SetPosition(Vector3D(30.0f, 0.0f, 50.0f));
	//mPlayer->SetBeyondSceneFlag(true);

	LoadCamera(mPlayer);

	LoadUI(mPlayer);
}

GameSceneStage2::~GameSceneStage2()
{
}
