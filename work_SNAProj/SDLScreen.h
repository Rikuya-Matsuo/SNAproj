#pragma once
#include "SDL/SDL.h"

// ロード画面をOpenGLの機能を使わずに描画するために作る。
// ロード画面以外で同様の仕組みを流用する予定はないが、念のため親クラスを作っておく。
// メインスレッドでは使えない。別スレッド上で使用すること。
class SDLScreen
{
public:
	SDLScreen();
	virtual ~SDLScreen();

	virtual void Update();

	virtual void Draw(SDL_Renderer * renderer);
};
