#include "Effect.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const float Effect::mDepth = -1.0f;

Effect::Effect(const std::string & texPath):
	mAppearSecond(1.0f),
	mTimer(0.0f)
{
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/board.gpmesh", this);
	MeshComponent * mc = new MeshComponent(this, 500);
	mc->SetMesh(mesh);
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
