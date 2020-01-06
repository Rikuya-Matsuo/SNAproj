#include "StageBase.h"
#include "Block.h"
#include "Floor.h"

float StageBase::mBlockScale = 1.0f;

StageBase::StageBase():
	mBlocks(nullptr),
	mBlockMassX(0),
	mBlockMassY(0),
	mFloorScale(1.0f)
{
}

StageBase::~StageBase()
{
	// メモリ削除
	if (mBlocks != nullptr)
	{
		for (Uint8 i = 0; i < mBlockMassY; ++i)
		{
			delete[] mBlocks[i];
		}
		delete[] mBlocks;
	}
}

void StageBase::Load(const std::string & fileName)
{
	return;
}

void StageBase::Construct()
{
	// ブロックがnullなら行わない
	if (mBlocks == nullptr)
	{
		SDL_Log("Stage : Block is not set! Fail to construction.\n");
		return;
	}
	// ステージの縦横ブロック数が設定されていない場合も行わない
	else if (mBlockMassX <= 0 || mBlockMassY <= 0)
	{
		SDL_Log("Stage : Block mass is invalid value. Fail to construction.\n");
	}

	///////////////////////////////////////
	// ブロック生成
	///////////////////////////////////////
	for (int yBlock = 0; yBlock < mBlockMassY; ++yBlock)
	{
		for (int xBlock = 0; xBlock < mBlockMassX; ++xBlock)
		{
			const Uint8 blockType = mBlocks[yBlock][xBlock];

			// ブロック配置データ内の値が0であれば生成しない
			if (!blockType)
			{
				continue;
			}

			// 生成
			Block * const bk = new Block(mBlockTexturePath);
			bk->SetScale(mBlockScale);

			// ブロックの高さの半分を計算
			const float blockHalfHeight = (Block::mModelSize / 2) * mBlockScale;

			// 一番下の段のブロックの上面が高さ0となるよう、生成する位置を計算
			Vector3D pos(Block::mModelSize * mBlockScale * xBlock, 0.0f, Block::mModelSize * mBlockScale * (mBlockMassY - (yBlock + 1)) - blockHalfHeight);

			// 位置情報代入
			bk->SetPosition(pos);
		}
	}

	////////////////////////////////////////////////
	// ブロックのさらに下に表示する床の生成
	////////////////////////////////////////////////
	// 生成位置を計算
	Vector3D flrPos;
	flrPos.x = mBlockMassX * Block::mModelSize * mBlockScale / 2.0f;
	flrPos.y = 0.0f;
	flrPos.z = mFloorHeight = -(Block::mModelSize * mBlockScale);

	// 生成
	Floor * const flr = new Floor(mFloorTexturePath);
	flr->SetPosition(flrPos);
	flr->SetScale(mFloorScale);
}
