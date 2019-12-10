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

	//InputMoveComponent * imc = new InputMoveComponent(this);

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

void Player::OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
	static char HitTest = 0;
	SDL_Log("Hit!%d\n", HitTest);
	HitTest ^= 1;
}

void Player::OnTouching(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
	static char touchingTest = 0;
	//SDL_Log("Touch!%d\n", touchingTest);
	touchingTest ^= 1;
}

void Player::OnApart(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
	static char apartTest = 0;
	SDL_Log("Apart!%d\n", apartTest);
	apartTest ^= 1;
}

void Player::UpdateActor()
{
}
