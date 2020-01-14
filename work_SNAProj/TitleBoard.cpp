#include "TitleBoard.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

TitleBoard::TitleBoard()
{
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadTexture("Assets/TitleKari.png", System::GetInstance().GetRenderer(), this);
	MeshComponent * mc = new MeshComponent(this, 500);
	mc->SetMesh(mesh);
}

TitleBoard::~TitleBoard()
{
}
