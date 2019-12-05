#include "Sprite.h"
#include <cmath>
#include <cstdio>

Sprite::Sprite():
	mPixels(nullptr)
{
}

Sprite::~Sprite()
{
}

void Sprite::Draw(const Vector2D& pos)
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
		Uint8 r, b, g, a;
		Uint32 surfacePixel = reinterpret_cast<Uint32*>(surface->pixels)[i];
		SDL_GetRGBA(surfacePixel, surface->format, &r, &b, &g, &a);

		// �s�N�Z��������
		// Windows�Ȃ炱����
		GLuint* pixel = &mPixels[pixelMass - (i + 1)];
		*pixel = 0;

		if (pixel == mPixels)
		{
			printf("");
		}

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

	// ���b�N����
	SDL_UnlockSurface(surface);
}
