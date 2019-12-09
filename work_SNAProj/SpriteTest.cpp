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
		msh->LoadTexture("Assets/Cube.png", System::GetInstance().GetRenderer());
		mc->SetMesh(msh);
	}
	else
	{
		SDL_Log("SpriteTest : Failed to load Mesh data.\n");
		delete mc;
	}

	//InputMoveComponent * imc = new InputMoveComponent(this);
}

SpriteTest::~SpriteTest()
{
}
