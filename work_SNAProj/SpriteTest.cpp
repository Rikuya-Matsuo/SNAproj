#include "System.h"
#include "SpriteTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "InputMoveComponent.h"
#include "BoxColliderComponent.h"

Texture * defaultTex = nullptr;

SpriteTest::SpriteTest():
	Actor()
{
	MeshComponent * mc = new MeshComponent(this);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	if (mMesh != nullptr)
	{
		mMesh->LoadTexture("Assets/Cube.png", System::GetInstance().GetRenderer(), this);
		mc->SetMesh(mMesh);
	}

	defaultTex = mMesh->GetTexture(this);
	
	//InputMoveComponent * imc = new InputMoveComponent(this);
	//imc->SetVerticalAxis(InputMoveComponent::AxisEnum_z);
	//imc->SetVerticalSpeed(imc->GetVerticalSpeed() * -1);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(mMesh->GetCollisionBox());

	mFlags &= ~mMovalFlagMask;
}

SpriteTest::~SpriteTest()
{
}

void SpriteTest::UpdateActor0()
{
	if (defaultTex != mMesh->GetTexture(this))
	{
		SDL_Delay(0);
	}
}

void SpriteTest::UpdateActor1()
{
	if (defaultTex != mMesh->GetTexture(this))
	{
		SDL_Delay(0);
	}
}

void SpriteTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	//SDL_Log("Hit!");
}
