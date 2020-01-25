#include "AnimationChips.h"
#include "System.h"
#include "Renderer.h"
#include "Texture.h"

#include <climits>

const AnimationChips::FlagType AnimationChips::mLoopEndFlagMask = 1 << 0;
const AnimationChips::FlagType AnimationChips::mStopFlagMask = 1 << 1;
std::unordered_map<std::string, std::vector<Texture*>> AnimationChips::mFrameTextureList;

AnimationChips::AnimationChips():
	mFlags(0),
	mCurrentTextureIndex(0),
	mSecondPerFrame(1.0f / 60),
	mCurrentRoutineIndex(0)
{
}

AnimationChips::~AnimationChips()
{
	mChipTextures.clear();
	mChipTextures.shrink_to_fit();
	std::vector<Texture*>().swap(mChipTextures);

	mRoutine.clear();
	mRoutine.shrink_to_fit();
	std::vector<size_t>().swap(mRoutine);
}

void AnimationChips::Update()
{
	// アニメーション停止のフラグが真の場合は更新を行わない
	if (mFlags & mStopFlagMask)
	{
		return;
	}

	// タイマー計算
	mTimer += System::GetInstance().GetDeltaTime();

	// フラグをリセット
	mFlags &= ~mLoopEndFlagMask;

	// タイマーに応じてフレームを進める
	if (mTimer >= mSecondPerFrame)
	{
		// タイマーをもとにアニメーションを何枚進めるかを計算
		int animProceed = static_cast<int>(mTimer / mSecondPerFrame);
		
		// 「0.0秒でコマを進める」指定は「1フレームごとにコマを進める」という意味と解釈する。
		if (mSecondPerFrame == 0.0f)
		{
			animProceed = 1;
		}

		// 表示するコマ順が指定されていない場合
		if (mRoutine.empty())
		{
			// コマを進める
			mCurrentTextureIndex += animProceed;

			// コマ数をオーバーしていれば0に戻す
			if (mCurrentTextureIndex >= mChipTextures.size())
			{
				mCurrentTextureIndex -= mChipTextures.size();

				// アニメーションのループが一周したことを示すフラグを立てる
				mFlags |= mLoopEndFlagMask;
			}
		}
		// 表示するコマ順が指定されている場合
		else
		{
			mCurrentRoutineIndex += animProceed;

			if (mCurrentRoutineIndex >= mRoutine.size())
			{
				mCurrentRoutineIndex -= mRoutine.size();

				mFlags |= mLoopEndFlagMask;
			}

			mCurrentTextureIndex = mRoutine[mCurrentRoutineIndex];
		}

		// コマを進めた分タイマーを減らす
		mTimer -= mSecondPerFrame * animProceed;
	}
}

void AnimationChips::Reset()
{
	mCurrentTextureIndex = 0;
	mTimer = 0.0f;

	// ループ終了フラグを初期化
	mFlags &= ~mLoopEndFlagMask;
}

size_t AnimationChips::Load(Renderer * renderer, const std::string & fileName, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, Texture * & texArray)
{
	// 検索し、同じ名前のファイルが読まれていた場合、その時のチップを渡す
	if (mFrameTextureList.find(fileName) != mFrameTextureList.end())
	{
		mChipTextures.reserve(mFrameTextureList[fileName].size());
		for (auto chip : mFrameTextureList[fileName])
		{
			mChipTextures.emplace_back(chip);
		}
		mChipTextures.shrink_to_fit();

		texArray = *mChipTextures.data();

		mSecondPerFrame = secondPerFrame;

		return mChipTextures.size();
	}

	// まだ読んだことのないファイルだった場合は読み込む
	SDL_Surface * srcSurface = IMG_Load(fileName.c_str());

	// 読み込みに失敗したなら関数から抜ける
	if (srcSurface == nullptr)
	{
		SDL_Log("Fail to Load image\n");
		return 0;
	}

	size_t sucessCount = 0;

	// サーフェイスをアンロック
	// サーフェイスの転写にはこれが重要である
	SDL_UnlockSurface(srcSurface);

	// 可読性のため、フォーマットのエイリアスを取得
	const SDL_PixelFormat * srcFormat = srcSurface->format;

	// テクスチャの配列を生成
	Texture * textures = new Texture[allNum];

	// マスクの取得
	Uint32 rMask, gMask, bMask, aMask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rMask = 0xFF000000;
	gMask = 0x00FF0000;
	bMask = 0x0000FF00;
	aMask = 0x000000FF;
#else
	rMask = 0x000000FF;
	gMask = 0x0000FF00;
	bMask = 0x00FF0000;
	aMask = 0xFF000000;
#endif

	// 縦横で二重ループ
	for (int y = 0; y < yNum; ++y)
	{
		for (int x = 0; x < xNum; ++x)
		{
			// サーフェイス生成
			SDL_Surface * dstSurface = SDL_CreateRGBSurface(0, chipW, chipH, 32, rMask, gMask, bMask, aMask);

			// 転写用矩形の設定
			SDL_Rect sRect, dRect;
			sRect.x = x * chipW;
			sRect.y = y * chipH;
			sRect.w = chipW;
			sRect.h = chipH;

			dRect.x = dRect.y = 0;

			// 転写！
			int errorCode = SDL_BlitSurface(srcSurface, &sRect, dstSurface, &dRect);
			if (errorCode)
			{
				SDL_Log("Fail to blit graph\n-----%s-----\n", SDL_GetError());

				delete[] textures;
				textures = nullptr;
				y = yNum - 1;
				break;
			}

			// 転写したサーフェイスをテクスチャに変換して格納
			textures[sucessCount].CreateFromSurface(dstSurface);
			mChipTextures.emplace_back(&textures[sucessCount]);

			// 返却値計算
			sucessCount++;

			// 転写先サーフェイスのメモリ解放
			SDL_free(dstSurface);

			// 履行回数が指定されたチップの総数以上になったらループを終了
			if (y * xNum + (x + 1) >= allNum)
			{
				// 上層のループ継続条件を未成立にすることで、条件評価時にループを終了させるという新たな技！
				// 利点：ループ終了フラグが必要ない上に、if文を省略できる。
				// 欠点：上層ループの変数をいじるのはいかがなものか
				y = yNum - 1;
				break;
			}
		}
	}

	// チップテクスチャ群の不要なメモリを解放
	mChipTextures.shrink_to_fit();

	// 移行同じ名前のチップが呼ばれたときに、チップのインスタンスを共有するように静的変数へ格納
	if (textures)
	{
		mFrameTextureList[fileName].reserve(mChipTextures.size());
		for (auto chip : mChipTextures)
		{
			mFrameTextureList[fileName].emplace_back(chip);
		}
		mFrameTextureList[fileName].shrink_to_fit();
	}

	// 転写元サーフェイスのメモリ解放
	SDL_free(srcSurface);

	// フレームごとの秒数を設定
	mSecondPerFrame = secondPerFrame;

	texArray = textures;
	return sucessCount;
}

void AnimationChips::SetTextureIndex(size_t num)
{
	if (num < mChipTextures.size())
	{
		mCurrentTextureIndex = num;
	}
	else
	{
		SDL_Delay(0);
	}
}

void AnimationChips::SetRoutine(const int * frameNumberArray, size_t arraySize)
{
	mRoutine.clear();
	for (size_t i = 0; i < arraySize; ++i)
	{
		if (frameNumberArray[i] < 0)
		{
			break;
		}
		mRoutine.emplace_back(frameNumberArray[i]);
	}
	mRoutine.shrink_to_fit();
}

void AnimationChips::SetRoutine(const int * frameNumberArray)
{
	SetRoutine(frameNumberArray, UINT32_MAX);
}
