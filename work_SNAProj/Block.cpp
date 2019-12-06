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
		SDL_Log("�u���b�N�F���b�V���̓ǂݍ��݂Ɏ��s\n");
	}

	mMeshComponent->SetMesh(msh);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	bcc->SetObjectBox(msh->GetCollisionBox());

	mScale *= 100.0f;

	InputMoveComponent * imc = new InputMoveComponent(this);
}

Block::~Block()
{
}
