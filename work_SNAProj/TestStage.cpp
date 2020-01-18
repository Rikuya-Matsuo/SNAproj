#include "TestStage.h"

static const bool plainStage = false;

const Uint8 TestStage::mTestBlockMassX = plainStage ? 128 : 32;
const Uint8 TestStage::mTestBlockMassY = 4;

TestStage::TestStage()
{
	if (!plainStage)
	{
		mTestBlocks = new Uint8*[mTestBlockMassY];
		for (Uint8 i = 0; i < mTestBlockMassY; ++i)
		{
			mTestBlocks[i] = new Uint8[mTestBlockMassX];
		}

		// コンストラクタ初期化子を使って効率をとるか、ローカル変数にいったん格納してコピーすることで可読性をとるか
		// 私が選んだのは後者だった。
		Uint8 testBlocks[mTestBlockMassY][mTestBlockMassX] =
		{
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
			{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1}
		};

		for (Uint8 i = 0; i < mTestBlockMassY; ++i)
		{
			SDL_memcpy(mTestBlocks[i], testBlocks[i], mTestBlockMassX);
		}
	}
	else
	{
		mTestBlocks = new Uint8*[mTestBlockMassY];
		for (Uint8 i = 0; i < mTestBlockMassY; ++i)
		{
			mTestBlocks[i] = new Uint8[mTestBlockMassX];
		}

		for (int yloop = 0; yloop < mTestBlockMassY; ++yloop)
		{
			for (int xloop = 0; xloop < mTestBlockMassX; ++xloop)
			{
				mTestBlocks[yloop][xloop] = (yloop == mTestBlockMassY - 1);
			}
		}
	}

	// ブロック、床のテクスチャファイル名を設定
	mBlockTexturePath = "Assets/SM_Ice_RuinedWalls.png";
	mBlockTexturePath.shrink_to_fit();

	mFloorTexturePath = "Assets/SM_Snow_Rock_Wall_A.png";
	mFloorTexturePath.shrink_to_fit();

	// 床オブジェクトのスケール値を設定
	mFloorScale = 10000.0f;
}

TestStage::~TestStage()
{
	for (int i = 0; i < mTestBlockMassY; ++i)
	{
		delete[] mTestBlocks[i];
	}
	delete[] mTestBlocks;
}

/*
void TestStage::Load(const std::string & fileName)
{
	/////////////////////////
	// ブロック並びをコピー
	/////////////////////////
	// メモリ確保
	mBlocks = new Uint8* [mTestBlockMassY];
	for (Uint8 i = 0; i < mTestBlockMassY; ++i)
	{
		mBlocks[i] = new Uint8[mTestBlockMassX];
	}

	// ステージの縦横のブロック数を設定
	mBlockMassX = mTestBlockMassX;
	mBlockMassY = mTestBlockMassY;

	// 内容コピー
	for (Uint8 yLoop = 0; yLoop < mTestBlockMassY; ++yLoop)
	{
		SDL_memcpy(mBlocks[yLoop], mTestBlocks[yLoop], mTestBlockMassX);
	}

	/////////////////////////
	// インスタンス生成
	/////////////////////////
	Construct();
}
*/
