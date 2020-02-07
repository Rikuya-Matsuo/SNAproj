#include "Effect.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const float Effect::mDepth = 0.1f;

Effect::Effect(const char * texPath, int priority):
	mAppearSecond(1.0f),
	mTimer(0.0f)
{
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	if (texPath)
	{
		mMesh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	}
	mMeshComp = new MeshComponent(this, 500);
	mMeshComp->SetMesh(mMesh);

	mFlags |= (mStopUpdateFlagMask_Base | mStopDrawFlagMask_Base);
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
		StopProcess();
	}

	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}

void Effect::OnBecomeActive()
{
	// タイマー初期化
	mTimer = 0.0f;

	// 加えて全コンポーネントアクティブ化
	Actor::OnBecomeActive();

	mFlags |= mCalculateTransformFlagMask_Base;
	mFlags &= ~mStopDrawFlagMask_Base;
}

void Effect::OnBecomeNotActive()
{
	Actor::OnBecomeNotActive();

	mMeshComp->SetActive(true);
}

void Effect::StopProcess()
{
	mFlags |= (mStopUpdateFlagMask_Base | mStopDrawFlagMask_Base);
}
