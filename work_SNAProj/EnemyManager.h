#pragma once
#include <string>

class StageBase;

class EnemyManager
{
public:
	EnemyManager(StageBase * stage);
	~EnemyManager();

	void LoadMapping(const std::string & path);

private:
	void GenerateEnemy(int type, int x, int y);

	StageBase * mStage;
};
