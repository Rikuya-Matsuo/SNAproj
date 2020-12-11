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

	void SetPlayer(Player * player) { mPlayer = player; }

protected:
	Player * mPlayer;

	UIScreen * mUI;

	void LoadStage(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);

	virtual void OnGoal();

	void LoadUI(Player * player);
};
