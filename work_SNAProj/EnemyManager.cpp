﻿#include "EnemyManager.h"
#include "EnemyWalker.h"
#include "Stage.h"
#include "AutoMoveComponent.h"
#include "Block.h"
#include "System.h"
#include <fstream>

#ifdef DEBUG_SNA
const int debugEnemyMassLimit = -1;
#endif

EnemyManager::EnemyManager(Stage * stage):
	mStage(stage)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::LoadMapping(const std::string & path)
{
	std::ifstream file;
	file.open(path.c_str());

	if (file.fail())
	{
		return;
	}

	int enemyMass = 0;
	int x;
	int y;
	x = y = 0;
	std::string buf;
	buf.reserve(4);
	char c;
	while (true)
	{
		c = file.get();

		if (file.eof())
		{
			break;
		}

#ifdef DEBUG_SNA

		if (debugEnemyMassLimit >= 0 && enemyMass >= debugEnemyMassLimit)
		{
			break;
		}

#endif // DEBUG_SNA

		if (c != ',' && c != '\n')
		{
			buf += c;
		}
		else
		{
			int type = std::stoi(buf);
			buf.clear();

			GenerateEnemy(type, x, y);
			if (type >= 0)
			{
				enemyMass++;
			}

			x++;

			if (c == '\n')
			{
				y++;
				x = 0;
			}

		}
	}

	file.close();
}

void EnemyManager::GenerateEnemy(int type, int inX, int inY)
{
	if (type < 0)
	{
		return;
	}

	// 生成
	EnemyWalker * enemy = new EnemyWalker;
	if (type == 1)
	{
		enemy->Flip();
	}

	// 位置計算
	const int yIndexFromLowest = (mStage->GetBlockMassY() - 1) - inY;
	const float blockSize = Stage::GetBlockScale() * Block::mModelSize;
	Vector3D pos = Vector3D::zero;
	pos.x = (inX * blockSize) + blockSize / 2.0f;
	pos.z = yIndexFromLowest * blockSize - blockSize / 2.0f;
	enemy->SetPosition(pos);
}
