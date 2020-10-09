#include "TitleCameraTargetActor.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "SceneBase.h"
#include "Stage.h"
#include "Block.h"

TitleCameraTargetActor::TitleCameraTargetActor():
	Actor(),
	mAutoMoveComponent(new AutoMoveComponent(this))
{
	SetAffectGravityFlag(false);

	mAutoMoveComponent->SetReverseFlag(true, false, false);
	mAutoMoveComponent->SetVelocity(Vector3D(1.0f, 0.0f, 0.0f));

	ClampSpeedComponent * clampComp = new ClampSpeedComponent(this);
	clampComp->SetClampDirectionFlags(true, true, true);
	float speedLimit = 30.0f;
	clampComp->SetLimit(Vector3D(speedLimit, speedLimit, speedLimit));
}

TitleCameraTargetActor::~TitleCameraTargetActor()
{
}

void TitleCameraTargetActor::UpdateActor0()
{
	bool goRightFlag = (mAutoMoveComponent->GetVelocity().x > 0.0f);
	// 右へ向かっている場合
	if (goRightFlag)
	{
		float line = mBelongScene->GetStage()->GetGoalLine();

		// ラインを超えているなら反転
		if (mPosition.x > line)
		{
			mAutoMoveComponent->ReverseVelocity();
		}
	}
	// 左へ向かっている場合
	else
	{
		float blockSize = mBelongScene->GetStage()->GetBlockScale() * Block::mModelSize;
		float line = blockSize * 3;

		// ラインを超えているなら反転
		if (mPosition.x < line)
		{
			mAutoMoveComponent->ReverseVelocity();
		}
	}
}
