#include "Block.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"

Block::Block():
	Actor()
{
	mMeshComponent = new MeshComponent(this);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/ForestGround01.gpmesh");
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
