#include "SpriteActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

SpriteActor::SpriteActor(const char * texPath)
{
	Mesh * msh = new Mesh;
	msh->Load("Assets/Board.gpmesh", System::GetInstance().GetRenderer(), this);
	if (texPath)
	{
		msh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	}
	MeshComponent * mc = new MeshComponent(this, 0, false, true);
	mc->SetMesh(msh);
}

SpriteActor::~SpriteActor()
{
}
