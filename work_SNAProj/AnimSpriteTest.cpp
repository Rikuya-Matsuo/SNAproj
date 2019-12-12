#include "AnimSpriteTest.h"
#include "System.h"
#include "BoxColliderComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"

AnimSpriteTest::AnimSpriteTest()
{
	MeshComponent * mc = new MeshComponent(this);
	mMesh = new Mesh();
	mMesh->Load("Assets/Board.gpmesh", System::GetInstance().GetRenderer());
	mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(),
		3, 3, 1, 32, 32, 1.0f, 0);
	mc->SetMesh(mMesh);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(mMesh->GetCollisionBox());
}

AnimSpriteTest::~AnimSpriteTest()
{
}

void AnimSpriteTest::OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt)
{
}
