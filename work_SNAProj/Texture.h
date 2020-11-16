#pragma once
#include <string>

struct SDL_Surface;

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);                      // テクスチャをファイルからロードする
	void Unload();                                               // テクスチャ解放処理
	void CreateFromSurface(SDL_Surface* surface);         // SDLサーフェスからテクスチャを作成する

	void SetActive();                                            // このテクスチャをアクティブ(ポリゴン描画で使用）にする

	int GetWidth() const { return mWidth; }                      // テクスチャ幅
	int GetHeight() const { return mHeight; }                    // テクスチャ高さ
private:
	unsigned int mTextureID;                                     // テクスチャID
	int mWidth;                                                  // テクスチャ幅
	int mHeight;                                                 // テクスチャ高さ
};
