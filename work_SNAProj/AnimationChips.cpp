#include "AnimationChips.h"
#include "Renderer.h"
#include "Texture.h"

const AnimationChips::FlagType AnimationChips::mLoopEndFlag = 1 << 0;

AnimationChips::AnimationChips():
	mFlags(0),
	mCurrentTextureIndex(0)
{
}

AnimationChips::~AnimationChips()
{
	
}

void AnimationChips::Update()
{
}

void AnimationChips::Reset()
{
	mCurrentTextureIndex = 0;
	mTimer = 0.0f;
}

size_t AnimationChips::Load(Renderer * renderer, const std::string & fileName, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, Texture * & texArray)
{
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

	// 縦横で二重ループ
	for (int y = 0; y < yNum; ++y)
	{
		for (int x = 0; x < xNum; ++x)
		{
			// サーフェイス生成
			SDL_Surface * dstSurface = SDL_CreateRGBSurface(0, chipW, chipH, 32,
				srcFormat->Rmask, srcFormat->Gmask, srcFormat->Bmask, srcFormat->Amask);

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

	// 転写元サーフェイスのメモリ解放
	SDL_free(srcSurface);

	texArray = textures;
	return sucessCount;
}
