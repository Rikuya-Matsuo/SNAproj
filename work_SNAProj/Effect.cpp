#include "Effect.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const float Effect::mDepth = 0.1f;

Effect::Effect(const std::string & texPath, int priority):
	mAppearSecond(1.0f),
	mTimer(0.0f)
{
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/board.gpmesh", this);
	mesh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	MeshComponent * mc = new MeshComponent(this, 500);
	mc->SetMesh(mesh);

	mFlags |= mStopUpdateFlagMask_Base;
	mFlags &= ~(mAffectGravityFlagMask_Base);

	if (mPriority != priority)
	{
		SetPriority(priority);
	}
}

Effect::~Effect()
{
}

void Effect::UpdateActor0()
{
	mTimer += System::GetInstance().GetDeltaTime();

	if (mTimer > mAppearSecond)
	{
		mFlags |= mStopUpdateFlagMask_Base;
	}

	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}

void Effect::OnBecomeActive()
{
	// �^�C�}�[������
	mTimer = 0.0f;

	// �����đS�R���|�[�l���g�A�N�e�B�u��
	Actor::OnBecomeActive();

	mFlags |= mCalculateTransformFlagMask_Base;
}
