#include "Player.h"
#include "Common.h"
#include "SpriteComponent.h"

Player::Player():
	Actor()
{
	SpriteComponent * spriteCmp = new SpriteComponent(this);
	spriteCmp->LoadTexture("Assets/Enemy01.png");
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}
