#include "System.h"
#include "SpriteTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "InputMoveComponent.h"
#include "BoxColliderComponent.h"

SpriteTest::SpriteTest():
	Actor()
{
	MeshComponent * mc = new MeshComponent(this);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh");
	if (msh != nullptr)
	{
		int texIndex = msh->LoadTexture("Assets/ForestBlock.png", System::GetInstance().GetRenderer());
		mc->SetMesh(msh);
		mc->SetTextureIndex(texIndex);
	}
	
	//InputMoveComponent * imc = new InputMoveComponent(this);
	//imc->SetVerticalAxis(InputMoveComponent::AxisEnum_z);
	//imc->SetVerticalSpeed(imc->GetVerticalSpeed() * -1);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(msh->GetCollisionBox());
}

SpriteTest::~SpriteTest()
{
}

void SpriteTest::OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
	//SDL_Log("Hit!");
}
