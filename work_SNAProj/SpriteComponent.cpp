#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(Actor * owner) :
	DrawComponentBase(owner, 100)
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
	// ï`âÊãÈå`ÇÃê›íË
	SDL_Rect rect;
	rect.x = static_cast<int>(mOwner->GetPosition().x);
	rect.y = static_cast<int>(mOwner->GetPosition().y);
	rect.w = static_cast<int>(mTextureWidth * mOwner->GetScale());
	rect.h = static_cast<int>(mTextureHeight * mOwner->GetScale());

	// ï`âÊ
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &rect);
}

void SpriteComponent::LoadTexture(const std::string & filePath)
{
	SDL_Surface* surface = nullptr;
	surface = IMG_Load(filePath.c_str());

	if (surface)
	{
		mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);

		SDL_QueryTexture(mTexture, nullptr, nullptr, &mTextureWidth, &mTextureHeight);
	}
	else
	{
		SDL_Log("Failed to load texture\n");
	}
}
