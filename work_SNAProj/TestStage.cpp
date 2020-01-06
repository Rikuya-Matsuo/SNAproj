#include "TestStage.h"

TestStage::TestStage()
{
	// コンストラクタ初期化子を使って効率をとるか、ローカル変数にいったん格納してコピーすることで可読性をとるか
	// 私が選んだのは後者だった。
	Uint8 testBlocks[mTestBlockMassY][mTestBlockMassX] =
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1}
	};

	// 動的メモリ確保を使わずに生成された二次元配列は、メモリ上に連続して並んでいるはずなので、一気にコピーできる。
	SDL_memcpy(mTestBlocks, testBlocks, mTestBlockMassX * mTestBlockMassY);

	// ブロック、床のテクスチャファイル名を設定
	mBlockTexturePath = "Assets/SM_Ice_RuinedWalls.png";
	mBlockTexturePath.shrink_to_fit();

	mFloorTexturePath = "Assets/SM_Snow_Rock_Wall_A.png";
	mFloorTexturePath.shrink_to_fit();

	// 床オブジェクトのスケール値を設定
	mFloorScale = 1000.0f;
}

TestStage::~TestStage()
{
}

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
