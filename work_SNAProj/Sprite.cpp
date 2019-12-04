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
	// ���b�N������
	SDL_LockSurface(surface);

	// �摜�T�C�Y��ݒ�
	mWidth = abs(surface->w);
	mHeight = abs(surface->h);

	// �������m��
	mPixels = new GLuint[mWidth * mHeight];

	// �s�N�Z���̃R�s�[
	for (unsigned int i = 0; i < mWidth * mHeight; ++i)
	{
		// �T�[�t�F�C�X�̃s�N�Z���F���擾
		Uint8 r, b, g, a;
		Uint32 surfacePixel = reinterpret_cast<Uint32*>(surface->pixels)[i];
		SDL_GetRGBA(surfacePixel, surface->format, &r, &b, &g, &a);

		// �s�N�Z��������
		mPixels[i] = 0;

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

			mPixels[i] |= color << ((3 - j) * 8);
		}
	}

	// ���b�N����
	SDL_UnlockSurface(surface);
}
