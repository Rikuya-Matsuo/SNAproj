#include "EnemyManager.h"
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
	// ファイルオープン
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
		// ストリームから文字を取得
		c = file.get();

		// ファイル終端に至ったとき、ループ終了
		if (file.eof())
		{
			break;
		}

		// デバッグ用のエネミー生成数上限に至ったかの評価
#ifdef DEBUG_SNA

		if (debugEnemyMassLimit >= 0 && enemyMass >= debugEnemyMassLimit)
		{
			break;
		}

#endif // DEBUG_SNA

		// カンマでも改行文字でもなければバッファに文字を記録
		if (c != ',' && c != '\n')
		{
			buf += c;
		}
		// csvセルの内容が終わりの場合
		else
		{
			// エネミータイプを取得
			int type = std::stoi(buf);
			buf.clear();

			// タイプに応じてエネミー生成
			GenerateEnemy(type, x, y);
			if (type >= 0)
			{
				// エネミーの数を記録
				enemyMass++;
			}

			// セルの座標を一つ右にずらす
			x++;

			// 改行文字だった場合
			if (c == '\n')
			{
				// セルの座標を一つ下に下げ、一番左に移動
				y++;
				x = 0;
			}

		}
	}

	// ファイルを閉じる
	file.close();
}

void EnemyManager::GenerateEnemy(int type, int inX, int inY)
{
	// エネミータイプが負の値の場合は、エネミーを生成しない
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
