#include "StageBase.h"
#include "Block.h"
#include "Floor.h"
#include <vector>
#include <fstream>

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
	mBlockMassX = mBlockMassY = 0;

	// ファイルオープン
	std::ifstream mapFile;
	mapFile.open(fileName.c_str());

	// 読み込み失敗時
	if (mapFile.fail())
	{
		return;
	}

	std::vector<int> numArray;		// 配列が決まるまで一時的にここに格納する
	std::string buf;				// 数字文字を格納
	buf.reserve(4);
	int mapWidth = 0;				// マップの幅を記録
	char c;							// ファイルの中の文字
	while (true)
	{
		// 文字を入れる
		c = mapFile.get();

		// ファイルが終わりならループを抜ける
		if (mapFile.eof())
		{
			break;
		}

		// カンマじゃなく、改行でもない = 数字文字
		if (c != ',' && c != '\n')
		{
			buf += c;
		}
		else
		{
			// マップ幅記録（マップ幅　=　カンマ、改行の読まれた数　より）
			mapWidth++;

			// 改行は段が変わったとき。
			if (c == '\n')
			{
				mBlockMassY++;

				// 一番広い幅を幅として記録する
				if (mapWidth > mBlockMassX)
				{
					mBlockMassX = mapWidth;
				}
				mapWidth = 0;
			}

			// 数字文字をint型の値として記録
			int num = std::stoi(buf);
			numArray.emplace_back(num);
			buf.clear();
		}
	}

	// メモリ確保
	mBlocks = new Uint8*[mBlockMassY];
	for (int i = 0; i < mBlockMassY; ++i)
	{
		mBlocks[i] = new Uint8[mBlockMassX];
	}

	// 代入
	for (int yloop = 0; yloop < mBlockMassY; ++yloop)
	{
		for (int xloop = 0; xloop < mBlockMassX; ++xloop)
		{
			int loopNum = yloop * mBlockMassX + xloop;
			mBlocks[yloop][xloop] = numArray[loopNum];
		}
	}

	std::vector<int>().swap(numArray);

	mapFile.close();

	// 生成
	Construct();
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
		return;
	}

	///////////////////////////////////////
	// ブロック生成
	///////////////////////////////////////
	const float blockSize = Block::mModelSize * mBlockScale;
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
			Block * const bk = new Block(mBlockTexturePath, yBlock == mBlockMassY - 1);
			bk->SetScale(mBlockScale);

			// ブロックの高さの半分を計算
			const float blockHalfHeight = blockSize / 2;

			// 一番下の段のブロックの上面が高さ0となるよう、生成する位置を計算
			// xも左が0となるように。
			Vector3D pos(
				Block::mModelSize * mBlockScale * xBlock + Block::mModelSize * mBlockScale / 2,
				0.0f,
				Block::mModelSize * mBlockScale * (mBlockMassY - (yBlock + 1)) - blockHalfHeight);

			// 位置情報代入
			bk->SetPosition(pos);
		}
	}

	// 終端3ブロックに踏み込んだ時点でゴールとする
	mGoalLineX = blockSize * (mBlockMassX - 3);

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
