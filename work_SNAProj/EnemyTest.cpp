#include "EnemyTest.h"
#include "Mesh.h"
#include "System.h"
#include "AnimationChips.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"

const EnemyTest::FlagType EnemyTest::mDamageAnimFlagMask = 1 << 0;

EnemyTest::EnemyTest():
	EnemyBase(3),
	mFlags_EnemyTest(0),
	mTextureIndex(0),
	mDamageAnimTimer(0.0f)
{
	mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.0f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	mAnimChips->StopPlaying();

	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(mMesh->GetCollisionBox());
}

EnemyTest::~EnemyTest()
{
}

void EnemyTest::UpdateActor0()
{
	if (mFlags_EnemyTest & mDamageAnimFlagMask)
	{
		mTextureIndex++;

		if (mTextureIndex >= 2)
		{
			mDamageAnimTimer += System::GetInstance().GetDeltaTime();
			const float damAniTime = 1.0f;

			if (mDamageAnimTimer >= damAniTime)
			{
				mDamageAnimTimer = 0.0f;
				mTextureIndex = 0;

				mFlags_EnemyTest &= ~mDamageAnimFlagMask;
			}
			else
			{
				mTextureIndex = 2;
			}
		}

		mAnimChips->SetTextureIndex(mTextureIndex);
	}
}

void EnemyTest::UpdateActor1()
{
	mPosition.y = 0.0f;
}

void EnemyTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (opponent->GetColliderAttribute() == ColliderAttribute::ColAtt_PlayerAttack)
	{
		mFlags_EnemyTest |= mDamageAnimFlagMask;

		SDL_Log("Damage!\n");
	}
}
