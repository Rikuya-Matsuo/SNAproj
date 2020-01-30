#include "SpriteActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

SpriteActor::SpriteActor(const char * texPath)
{
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	if (texPath)
	{
		msh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	}
	MeshComponent * mc = new MeshComponent(this, 0, false, true);
	mc->SetMesh(msh);

	mFlags &= ~mAffectGravityFlagMask_Base;
}

SpriteActor::~SpriteActor()
{
}

void SpriteActor::UpdateActor1()
{
	return;
}
