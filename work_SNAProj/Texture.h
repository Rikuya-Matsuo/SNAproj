// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);                      // テクスチャをファイルからロードする
	void Unload();                                               // テクスチャ解放処理
	void CreateFromSurface(struct SDL_Surface* surface);         // SDLサーフェスからテクスチャを作成する

	void SetActive();                                            // このテクスチャをアクティブ(ポリゴン描画で使用）にする

	int GetWidth() const { return mWidth; }                      // テクスチャ幅
	int GetHeight() const { return mHeight; }                    // テクスチャ高さ
private:
	unsigned int mTextureID;                                     // テクスチャID
	int mWidth;                                                  // テクスチャ幅
	int mHeight;                                                 // テクスチャ高さ
};
