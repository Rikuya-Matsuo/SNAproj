#include "Player.h"
#include "Common.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"
#include "Mesh.h"
#include "System.h"
#include "Renderer.h"

Player::Player():
	Actor()
{
	// メッシュのロード
	MeshComponent * mc = new MeshComponent(this);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh");
	mMesh->LoadDivTexture("Assets/NinjaStay.png", System::GetInstance().GetRenderer(),
		10, 10, 1, 128, 128, 0.07f, 0);
	mc->SetMesh(mMesh);

	// コライダーの設定
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Player);
	bcc->SetObjectBox(mMesh->GetCollisionBox());

	//InputMoveComponent * imc = new InputMoveComponent(this);

	// 落下スピード割合の調整
	mFallSpeedRate = 0.01f;
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
