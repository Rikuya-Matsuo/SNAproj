#include "Player.h"
#include "Common.h"
#include "SpriteComponent.h"
#include "InputMoveComponent.h"

Player::Player():
	Actor()
{
	SpriteComponent * spriteCmp = new SpriteComponent(this);
	spriteCmp->LoadTexture("Assets/Enemy01.png");

	InputMoveComponent * imc = new InputMoveComponent(this);
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}
