#include "GameOverBoard.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "System.h"
#include <typeinfo>

GameOverBoard::GameOverBoard():
	TitleBoard()
{
	Mesh * mesh = const_cast<Mesh*>(mMeshComp->GetMesh());
	mesh->LoadTexture("Assets/GameOver.png", System::GetInstance().GetRenderer(), this);
}

GameOverBoard::~GameOverBoard()
{
}
