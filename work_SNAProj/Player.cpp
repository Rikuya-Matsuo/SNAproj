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

	mInputComponent = new InputMoveComponent(this);
	mInputComponent->SetVerticalAxis(mInputComponent->AxisEnum_z);
	
	const float speed = 7500.0f;
	mInputComponent->SetHorizontalSpeed(speed);
	mInputComponent->SetVerticalSpeed(-speed);

	// 落下スピード割合の調整
	mFallSpeedRate = 0.01f;

	mFlags |= mPlayerFlagMask;
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor1()
{
	// ブレーキ
	if (!mInputComponent->GetHorizonInputFlag())
	{
		mMoveVector.x *= 0.05f;

		if (fabsf(mMoveVector.x) < 0.001f)
		{
			mMoveVector.x = 0.0f;
		}
	}

	if (!mInputComponent->GetVerticalInputFlag())
	{
		mMoveVector.z *= 0.05f;

		if (fabsf(mMoveVector.z) < 0.001f)
		{
			mMoveVector.z = 0.0f;
		}
	}

	// 奥行きの情報を常に0に
	mPosition.y = 0.0f;

	// 基底クラスのほうも呼ぶ
	Actor::UpdateActor1();
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
