#pragma once
#include "SceneBase.h"
#include "System.h"

class Player;
class UIScreen;

class GameScene : public SceneBase
{
public:
	GameScene(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);
	virtual ~GameScene();

	void Update() override;

	bool IsGameScene() const override final { return true; }

	void TellStageClear() { mGoalFlag = true; }

protected:
	static Player * mPlayer;

	UIScreen * mUI;

	void LoadStage(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);

	void LoadUI(Player * player);

	void LoadEnemy(const std::string & enemyMappingDataPath);

	virtual void OnGoal();

	virtual void LoadCamera(Player * player);

private:
	bool mGoalFlag;
};

#ifdef DEBUG_SNA

// テストシーンを生成するマクロ
// 本番用シーンを生成する際はコメントアウト
//#define GEN_TEST_SCENE

#endif // DEBUG_SNA
