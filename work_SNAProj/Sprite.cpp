#include "Sprite.h"
#include <cmath>

Sprite::Sprite():
	mPixels(nullptr)
{
}

Sprite::~Sprite()
{
}

void Sprite::Draw()
{
}

void Sprite::ConvertSDLSurface(SDL_Surface * surface)
{
	// ロックを解除
	SDL_LockSurface(surface);

	// 画像サイズを設定
	mWidth = abs(surface->w);
	mHeight = abs(surface->h);

	// メモリ確保
	mPixels = new GLuint[mWidth * mHeight];

	// ピクセルのコピー
	for (unsigned int i = 0; i < mWidth * mHeight; ++i)
	{
		// サーフェイスのピクセル色を取得
		Uint8 r, b, g, a;
		Uint32 surfacePixel = reinterpret_cast<Uint32*>(surface->pixels)[i];
		SDL_GetRGBA(surfacePixel, surface->format, &r, &b, &g, &a);

		// ピクセル初期化
		mPixels[i] = 0;

		// コピー
		for (char j = 0; j < 4; ++j)
		{
			GLuint color;
			switch (j)
			{
			case 0:
				color = r;
				break;
			case 1:
				color = g;
				break;
			case 2:
				color = b;
				break;
			case 3:
				color = a;
				break;
			default:
				break;
			}

			mPixels[i] |= color << ((3 - j) * 8);
		}
	}

	// ロック解除
	SDL_UnlockSurface(surface);
}
