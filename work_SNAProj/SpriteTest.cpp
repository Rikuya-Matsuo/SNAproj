#include "System.h"
#include "SpriteTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "InputMoveComponent.h"
#include "BoxColliderComponent.h"

SpriteTest::SpriteTest():
	Actor()
{
	MeshComponent * mc = new MeshComponent(this, 100);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/ninjastayOrigin.gpmesh", this);
	if (mMesh != nullptr)
	{
		mMesh->LoadTexture("Assets/NinjaStay01.png", System::GetInstance().GetRenderer(), this);
		mMesh->SetIsBoardFlagTrue();
		mc->SetMesh(mMesh);
	}

	//InputMoveComponent * imc = new InputMoveComponent(this);
	//imc->SetVerticalAxis(InputMoveComponent::AxisEnum_z);
	//imc->SetVerticalSpeed(imc->GetVerticalSpeed() * -1);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(mMesh->GetCollisionBox());

	mFlags &= ~mMovalFlagMask_Base;
}

SpriteTest::~SpriteTest()
{
}

void SpriteTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	//SDL_Log("Hit!");
}
