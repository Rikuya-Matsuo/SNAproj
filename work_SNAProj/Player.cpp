#include "Player.h"
#include "Common.h"
#include "SkeletalMeshComponent.h"
#include "InputMoveComponent.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "System.h"
#include "Renderer.h"

Player::Player():
	Actor()
{
	// メッシュのロード
	mMeshComponent = new SkeletalMeshComponent(this);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/SK_Mannequin.gpmesh");
	mMeshComponent->SetMesh(msh);

	// スケルトンのロード
	mMeshComponent->SetSkeleton(System::GetInstance().GetRenderer()->GetSkeleton("Assets/SK_Mannequin.gpskel"));

	InputMoveComponent * imc = new InputMoveComponent(this);
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}

void Player::Update()
{
	CalculateWorldTransform();

	UpdateComponents();
	UpdateActor();

	CalculateWorldTransform();
}

void Player::UpdateActor()
{
}
