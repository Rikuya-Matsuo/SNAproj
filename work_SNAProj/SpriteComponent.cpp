#include "SpriteComponent.h"
#include "Sprite.h"
#include "Input.h"

SpriteComponent::SpriteComponent(Actor * owner) :
	DrawComponentBase(owner, 100)
	//mSprite(nullptr)
{
}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::Update()
{
}

void SpriteComponent::Draw() const
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE))
	{
		printf("");
	}

	// 描画矩形の設定
	SDL_Rect rect;
	rect.x = static_cast<int>(mOwner->GetPosition().x);
	rect.y = static_cast<int>(mOwner->GetPosition().y);
	rect.w = static_cast<int>(mTextureWidth * mOwner->GetScale());
	rect.h = static_cast<int>(mTextureHeight * mOwner->GetScale());

	// 描画
	// すでにOpenGLの描画システムを使っているため、画面に描画されてくれない
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &rect);

	/*
	if (mSprite)
	{
		mSprite->Draw(mOwner->GetPosition());
	}
	*/
}

void SpriteComponent::LoadSprite(const std::string & filePath)
{
	SDL_Surface* surface = nullptr;
	surface = IMG_Load(filePath.c_str());

	// 失敗なら関数を終了
	if (!surface)
	{
		SDL_Log("wesrctgyhjnu\n");
		return;
	}

	// テクスチャ使用
	{
		mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);

		SDL_QueryTexture(mTexture, nullptr, nullptr, &mTextureWidth, &mTextureHeight);
	}

	//// スプライト使用
	//{
	//	// 元のスプライトを削除
	//	if (mSprite)
	//	{
	//		delete mSprite;
	//	}

	//	// 新たなスプライトを生成＆SDL_Surfaceから変換
	//	mSprite = new Sprite;
	//	mSprite->ConvertSDLSurface(surface);
	//}

	// メモリ解放
	SDL_FreeSurface(surface);
}
