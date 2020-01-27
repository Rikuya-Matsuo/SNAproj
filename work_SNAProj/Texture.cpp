// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Texture.h"
#include "System.h"
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

Texture::Texture()
	:mTextureID(0)
	, mWidth(0)
	, mHeight(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const std::string& fileName)
{
	int channels = 0;

	// SDLサーフェスをテクスチャから作成
	SDL_Texture* tex = nullptr;
	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (!surf)
	{
		printf("テクスチャ読み込みに失敗 %s", fileName.c_str());
		return false;
	}

	// 透過を有効にするため、一度BlitSurfaceしてみる。
	//BlitSurfaceProcess(surf);

	// サーフェスからテクスチャを作る
	tex = SDL_CreateTextureFromSurface(System::GetInstance().GetSDLRenderer(), surf);
	if (!tex)
	{
		printf("サーフェスからテクスチャの作成に失敗 : %s", fileName.c_str());
		return false;
	}

	// 画像の幅、高さを取得
	mWidth = surf->w;
	mHeight = surf->h;
	channels = surf->format->BytesPerPixel;
	
	// OpenGLにテクスチャ登録
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format,
		GL_UNSIGNED_BYTE, surf->pixels);

	// SDLサーフェスは用済みなので解放
	SDL_FreeSurface(surf);

	// テクスチャフィルタリング設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;

	// Generate a GL texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	// Use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::BlitSurfaceProcess(SDL_Surface *& surface)
{
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

	SDL_Surface * blited =
		SDL_CreateRGBSurface(0, surface->w, surface->h, 32, rMask, gMask, bMask, aMask);

	SDL_Rect sRect, dRect;
	sRect.x = sRect.y = 0;
	sRect.w = surface->w;
	sRect.h = surface->h;

	dRect.x = dRect.y = 0;

	int errorCode = SDL_BlitSurface(surface, &sRect, blited, &dRect);
	if (errorCode)
	{
		SDL_Log("Fail to blit graph\n-----%s-----\n", SDL_GetError());
		SDL_FreeSurface(blited);
		return;
	}

	SDL_FreeSurface(surface);
	surface = blited;
}
