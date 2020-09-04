#pragma once
#include <string>

class Stage;

class EnemyManager
{
public:
	EnemyManager(Stage * stage);
	~EnemyManager();

	void LoadMapping(const std::string & path);

private:
	void GenerateEnemy(int type, int x, int y);

	Stage * mStage;
};
