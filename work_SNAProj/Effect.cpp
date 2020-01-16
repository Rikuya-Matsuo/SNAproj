#include "Effect.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const float Effect::mDepth = 0.1f;

Effect::Effect(const std::string & texPath):
	mAppearSecond(1.0f),
	mTimer(0.0f)
{
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/board.gpmesh", this);
	mesh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	MeshComponent * mc = new MeshComponent(this, 500);
	mc->SetMesh(mesh);

	mFlags |= mStopUpdateFlagMask_Base;
	mFlags &= ~(mAffectGravityFlagMask_Base);
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
	// タイマー初期化
	mTimer = 0.0f;

	// 加えて全コンポーネントアクティブ化
	Actor::OnBecomeActive();

	mFlags |= mCalculateTransformFlagMask_Base;
}
