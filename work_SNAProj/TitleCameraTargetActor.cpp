#include "TitleCameraTargetActor.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "SceneBase.h"
#include "Stage.h"
#include "Block.h"
#include "System.h"
#include "Input.h"

TitleCameraTargetActor::TitleCameraTargetActor():
	Actor(),
	mAutoMoveComponent(new AutoMoveComponent(this))
{
	// 重力不適用
	SetAffectGravityFlag(false);

	// 自動移動の設定
	mAutoMoveComponent->SetReverseFlag(true, false, false);
	mAutoMoveComponent->SetVelocity(Vector3D(1.0f, 0.0f, 0.0f));

	// 制限速度の設定
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
	bool goRightFlag = !(mAutoMoveComponent->IsNowReverse());
	float blockSize = mBelongScene->GetStage()->GetBlockScale() * Block::mModelSize;

	// デバッグ用に、スペースキーで進行方向へ一定距離ワープする
#ifdef DEBUG_SNA
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE))
	{
		float warpStride = blockSize * 10;
		mPosition.x += (goRightFlag) ? warpStride : -warpStride;
	}
#endif // DEBUG_SNA

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
		float line = blockSize * 3;

		// ラインを超えているなら反転
		if (mPosition.x < line)
		{
			mAutoMoveComponent->ReverseVelocity();
		}
	}
}
