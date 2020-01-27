#include "TitleBoard.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "AnimationChips.h"
#include "CompletionMeshActor.h"
#include "System.h"

TitleBoard::TitleBoard()
{
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	//mMesh->LoadTexture("Assets/TitleLogo.png", System::GetInstance().GetRenderer(), this);
	mMesh->LoadDivTexture("Assets/TitleLogo.png", System::GetInstance().GetRenderer(), this, 4, 2, 2, 512, 512, 0.0f, 0);

	AnimationChips * ac = mMesh->GetAnimChips(this, 0);
	ac->StopPlaying();

	Vector3D size = mMesh->GetCollisionBox().mMax - mMesh->GetCollisionBox().mMin;

	for (size_t i = 0; i < mMeshActorMass; ++i)
	{
		ac->SetTextureIndex(i);

		mMeshActor[i] = new CompletionMeshActor(this, 500);
		mMeshActor[i]->SetTexture(ac->GetCurrentTexture());

		float offsetX = size.x / 2;
		float offsetY = size.z / 2;

		Vector3D pos = Vector3D::zero;
		pos.x += (i & 1) ? offsetX : -offsetX;
		pos.z += (i < 2) ? offsetY : -offsetY;
		mMeshActor[i]->SetPositionOffset(pos);

		mMeshActor[i]->SetSyncScaleFlag(false);
	}

	mFlags &= ~mAffectGravityFlagMask_Base;
}

TitleBoard::~TitleBoard()
{
}

void TitleBoard::UpdateActor0()
{
	if (mMeshActor[0]->GetScale() != mScale)
	{
		Vector3D size = mMesh->GetCollisionBox().mMax - mMesh->GetCollisionBox().mMin;

		float offsetX = size.x * mScale / 2 / 2;
		float offsetY = size.z * mScale / 2 / 2;

		for (size_t i = 0; i < mMeshActorMass; ++i)
		{
			Vector3D pos = Vector3D::zero;
			pos.x += (i & 1) ? offsetX : -offsetX;
			pos.z += (i < 2) ? offsetY : -offsetY;
			mMeshActor[i]->SetPositionOffset(pos);

			mMeshActor[i]->SetScale(mScale / (mMeshActorMass / 2));
		}
	}
}
