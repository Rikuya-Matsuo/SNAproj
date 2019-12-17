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

	// 静的確保(？)された二次元配列は、メモリ上に連続して並んでいるはずなので、一気にコピーできる。
	SDL_memcpy(mTestBlocks, testBlocks, mTestBlockMassX * mTestBlockMassY);
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
