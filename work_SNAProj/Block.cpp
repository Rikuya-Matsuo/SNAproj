#include "Block.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"

const float Block::mModelSize = 100.0f;

Block::Block(const std::string & texturePath):
	Actor()
{
	mMeshComponent = new MeshComponent(this, 100);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Cube.gpmesh", this);
	msh->LoadTexture(texturePath, System::GetInstance().GetRenderer(), this);
	if (!msh)
	{
		SDL_Log("ブロック：メッシュの読み込みに失敗\n");
	}

	mMeshComponent->SetMesh(msh);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	bcc->SetObjectBox(msh->GetCollisionBox());

	// 重力落下を行わない。ヒット時に押し返されない。
	mFlags &= ~(mAffectGravityFlagMask | mMovalFlagMask);
}

Block::~Block()
{
}
