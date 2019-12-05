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
	// ���b�N������
	SDL_LockSurface(surface);

	// �摜�T�C�Y��ݒ�
	mWidth = abs(surface->w);
	mHeight = abs(surface->h);

	// �s�N�Z�����v�Z
	const unsigned int pixelMass = mWidth * mHeight;

	// �������m��
	mPixels = new GLuint[pixelMass];

	// �s�N�Z���̃R�s�[
	for (unsigned int i = 0; i < pixelMass; ++i)
	{
		// �T�[�t�F�C�X�̃s�N�Z���F���擾
		Uint8 r, g, b, a;
		Uint32 surfacePixel = reinterpret_cast<Uint32*>(surface->pixels)[i];
		SDL_GetRGBA(surfacePixel, surface->format, &r, &g, &b, &a);

		// �K�i���Ⴄ�H255��������Ă݂�
		auto adjust = [](Uint8 & value) { value = 255 - value; };
		adjust(r);
		adjust(g);
		adjust(b);

		if (a < 16)
		{
			r = g = b = 0;
		}

		// �s�N�Z��������
		// Windows�Ȃ炱����
		GLuint* pixel = &mPixels[pixelMass - (i + 1)];
		*pixel = 0;

		// �R�s�[
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

	// �������]���ꂽ��ԂŃ��[�h����Ă���̂Ŕ��]
	XFlip();

	// ���b�N����
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
