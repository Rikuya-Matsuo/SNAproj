#include "Sprite.h"
#include <cmath>
#include <cstdio>

const Sprite::FlagType Sprite::mXFlipFlagMask = 1 << 0;
//const Sprite::FlagType Sprite::mPrevXFlipFlagMask = 1 << 1;

Sprite::Sprite():
	mPixels(nullptr),
	mFlags(0)
{
}

Sprite::~Sprite()
{
}

void Sprite::Draw(const Vector2D& pos) const
{
	glRasterPos2f(pos.x, pos.y);
	glDrawPixels(mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, mPixels);
}

void Sprite::ConvertSDLSurface(SDL_Surface * surface)
{
	// ロックを解除
	SDL_LockSurface(surface);

	// 画像サイズを設定
	mWidth = abs(surface->w);
	mHeight = abs(surface->h);

	// ピクセル数計算
	const unsigned int pixelMass = mWidth * mHeight;

	// メモリ確保
	mPixels = new GLuint[pixelMass];

	// ピクセルのコピー
	for (unsigned int i = 0; i < pixelMass; ++i)
	{
		// サーフェイスのピクセル色を取得
		Uint8 r, g, b, a;
		Uint32 surfacePixel = reinterpret_cast<Uint32*>(surface->pixels)[i];
		SDL_GetRGBA(surfacePixel, surface->format, &r, &g, &b, &a);

		// 規格が違う？255から引いてみる
		auto adjust = [](Uint8 & value) { value = 255 - value; };
		adjust(r);
		adjust(g);
		adjust(b);

		if (a < 16)
		{
			r = g = b = 0;
		}

		// ピクセル初期化
		// Windowsならこっち
		GLuint* pixel = &mPixels[pixelMass - (i + 1)];
		*pixel = 0;

		if (pixel == mPixels)
		{
			printf("");
		}

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

			*pixel |= color << ((3 - j) * 8);
		}
	}

	// 横が反転された状態でロードされているので反転
	XFlip();

	// ロック解除
	SDL_UnlockSurface(surface);
}

void Sprite::XFlip()
{
	for (unsigned int i = 0; i < mHeight; ++i)
	{
		for (unsigned int j = 0; j < mWidth / 2; ++j)
		{
			unsigned int element[2];
			element[0] = i * mWidth + j;
			element[1] = i * mWidth + (mWidth - (j + 1));
			GLuint tmp = mPixels[element[0]];
			mPixels[element[0]] = mPixels[element[1]];
			mPixels[element[1]] = tmp;
		}
	}

	if (mFlags & mXFlipFlagMask)
	{
		mFlags &= ~mXFlipFlagMask;
	}
	else
	{
		mFlags |= mXFlipFlagMask;
	}
}
