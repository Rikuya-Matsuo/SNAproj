#pragma once
#include "SceneBase.h"

class Player;
class UIScreen;

class GameScene : public SceneBase
{
public:
	GameScene(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);
	virtual ~GameScene();

	void Update() override;

	bool IsGameScene() const override final { return true; }

	void TellStageClear() { mClearFlag = true; }

protected:
	static Player * mPlayer;

	UIScreen * mUI;

	void LoadStage(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);

	void LoadUI(Player * player);

	void LoadEnemy(const std::string & enemyMappingDataPath);

	virtual void OnGoal();

	virtual void LoadCamera(Player * player);

private:
	bool mClearFlag;
};
