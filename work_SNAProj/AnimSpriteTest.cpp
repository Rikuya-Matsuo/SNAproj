#include "AnimSpriteTest.h"
#include "System.h"
#include "Input.h"
#include "BoxColliderComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "AnimationChips.h"

AnimSpriteTest::AnimSpriteTest()
{
	MeshComponent * mc = new MeshComponent(this, 100);
	mMesh = new Mesh();
	mMesh->Load("Assets/Board.gpmesh", System::GetInstance().GetRenderer(), this);
	bool sucess = mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this,
		3, 3, 1, 32, 32, 0.1f, 0);
	if (!sucess)
	{
		SDL_Log("Fail to load animation chips\n");
	}
	mc->SetMesh(mMesh);

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(mMesh->GetCollisionBox());
}

AnimSpriteTest::~AnimSpriteTest()
{
}

void AnimSpriteTest::UpdateActor1()
{
	if (mMesh->GetAnimLoopEndFlag())
	{
		AnimationChips * animChips = mMesh->GetAnimChips(this, 0);
		if (animChips != nullptr)
		{
			animChips->Reset();

			animChips->StopPlaying();
		}
	}

	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_A))
	{
		AnimationChips * animChips = mMesh->GetAnimChips(this, 0);
		if (animChips != nullptr)
		{
			animChips->StartPlaying();
		}
	}

	mPosition.y = 0.0f;
}

void AnimSpriteTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}
