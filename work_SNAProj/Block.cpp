#include "Block.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"

const float Block::mModelSize = 1.0f;

Block::Block():
	Actor()
{
	mMeshComponent = new MeshComponent(this, 100);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Cube.gpmesh", this);
	msh->LoadTexture("Assets/SM_Snow_Rock_Wall_A.png", System::GetInstance().GetRenderer(), this);
	if (!msh)
	{
		SDL_Log("ブロック：メッシュの読み込みに失敗\n");
	}

	mMeshComponent->SetMesh(msh);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	bcc->SetObjectBox(msh->GetCollisionBox());

	// 重力の影響を受けない
	SetAffectGravityFlag(false);

	// ヒット時に押し返されない
	mFlags &= ~mMovalFlagMask;
}

Block::~Block()
{
}
