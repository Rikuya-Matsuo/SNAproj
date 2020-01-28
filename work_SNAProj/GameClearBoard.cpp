#include "GameClearBoard.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

GameClearBoard::GameClearBoard()
{
	Mesh * mesh = const_cast<Mesh*>(mMeshComp->GetMesh());
	mesh->LoadTexture("Assets/GameClear.png", System::GetInstance().GetRenderer(), this);
}


GameClearBoard::~GameClearBoard()
{
}
