#include "EnemyManager.h"
#include "EnemyTest.h"
#include "StageBase.h"
#include "AutoMoveComponent.h"
#include "Block.h"
#include <fstream>

EnemyManager::EnemyManager(StageBase * stage):
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

	// w, hいらないかも
	int w;
	int h;
	w = h = 0;
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

		if (c != ',' && c != '\n')
		{
			buf += c;
		}
		else
		{
			int type = std::stoi(buf);
			buf.clear();

			GenerateEnemy(type, x, y);

			x++;

			if (c == '\n')
			{
				y++;

				if (x > w)
				{
					w = x;
					x = 0;
				}
			}

		}
	}
	h = y;

	file.close();
}

void EnemyManager::GenerateEnemy(int type, int inX, int inY)
{
	if (type < 0)
	{
		return;
	}

	// 生成
	EnemyTest * enemy = new EnemyTest;
	if (type == 1)
	{
		enemy->Flip();
		enemy->mAutoMoveComp->ReverseVelocity();
		enemy->mFlags_Enemy |= EnemyTest::mLookRightFlagMask_EBase;
	}

	// 位置計算
	const int yIndexFromLowest = (mStage->GetBlockMassY() - 1) - inY;
	const float blockSize = StageBase::GetBlockScale() * Block::mModelSize;
	Vector3D pos = Vector3D::zero;
	pos.x = (inX * blockSize) + blockSize / 2.0f;
	pos.z = yIndexFromLowest * blockSize - blockSize / 2.0f;
	enemy->SetPosition(pos);
}
