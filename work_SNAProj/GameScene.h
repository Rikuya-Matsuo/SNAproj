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

protected:
	static Player * mPlayer;

	UIScreen * mUI;

	void LoadStage(const std::string & mapFilePath, const std::string & blockTexPath, const std::string & floorTexPath);

	void LoadUI(Player * player);

	virtual void OnGoal();

	virtual void LoadCamera(Player * player);
};
