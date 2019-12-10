#include "System.h"
#include "SpriteTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "InputMoveComponent.h"

SpriteTest::SpriteTest():
	Actor()
{
	MeshComponent * mc = new MeshComponent(this);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh");
	if (msh != nullptr)
	{
		int texIndex = msh->LoadTexture("Assets/ForestBlock.png", System::GetInstance().GetRenderer());
		mc->SetMesh(msh);
		mc->SetTextureIndex(texIndex);
	}
	//InputMoveComponent * imc = new InputMoveComponent(this);
}

SpriteTest::~SpriteTest()
{
}
