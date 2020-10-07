#include "SpriteActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

SpriteActor::SpriteActor(const char * texPath, int drawOrder):
	mMesh(nullptr)
{
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	if (texPath)
	{
		mMesh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	}
	MeshComponent * mc = new MeshComponent(this, drawOrder, false);
	mc->SetMesh(mMesh);

	mFlags &= ~mAffectGravityFlagMask_Base;
}

SpriteActor::~SpriteActor()
{
}

void SpriteActor::UpdateActor1()
{
	return;
}
