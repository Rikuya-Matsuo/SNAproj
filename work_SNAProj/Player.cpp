#include "Player.h"
#include "Common.h"
#include "SkeletalMeshComponent.h"
#include "InputMoveComponent.h"

Player::Player():
	Actor()
{
	SkeletalMeshComponent * smc = new SkeletalMeshComponent(this);

	InputMoveComponent * imc = new InputMoveComponent(this);
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}

void Player::Update()
{
	UpdateComponents();
	UpdateActor();
}

void Player::UpdateActor()
{
	
}
