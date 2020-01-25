#include "TitleBoard.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

TitleBoard::TitleBoard()
{
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadTexture("Assets/TitleLogo.png", System::GetInstance().GetRenderer(), this);
	//mesh->LoadDivTexture("Assets/TitleLogo.png", System::GetInstance().GetRenderer(), this, 1, 1, 1, 1041, 1041, 0.0f, 0);
	mMeshComp = new MeshComponent(this, 500);
	mMeshComp->SetMesh(mesh);

	mFlags &= ~mAffectGravityFlagMask_Base;
}

TitleBoard::~TitleBoard()
{
}
