#include "Player.h"
#include "Common.h"
#include "SkeletalMeshComponent.h"
#include "BoxColliderComponent.h"
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

	// コライダーの設定
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Player);
	bcc->SetObjectBox(msh->GetCollisionBox());

	InputMoveComponent * imc = new InputMoveComponent(this);

	// スケール値の調整
	mScale = 0.5f;
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
