#include "Effect.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const float Effect::mDepth = 0.1f;

Effect::Effect(const char * texPath, int priority):
	mAppearSecond(1.0f),
	mTimer(0.0f)
{
	// メッシュのロード
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	if (texPath)
	{
		mMesh->LoadTexture(texPath, System::GetInstance().GetRenderer(), this);
	}
	mMeshComp = new MeshComponent(this, 500);
	mMeshComp->SetMesh(mMesh);

	// アップデート停止フラグと描画停止フラグを真にする
	mFlags |= (mStopUpdateFlagMask_Base | mStopDrawFlagMask_Base);
	// 重力を無効化
	mFlags &= ~(mAffectGravityFlagMask_Base);

	// 更新優先度を設定
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
	// タイマー更新
	mTimer += System::GetInstance().GetDeltaTime();

	// 設定した時間が過ぎたとき、更新と描画を停止
	if (mTimer > mAppearSecond)
	{
		StopProcess();
	}

	// 奥行きの調節
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

	// 行列計算フラグを真にし、描画停止フラグを偽にする
	mFlags |= mCalculateTransformFlagMask_Base;
	mFlags &= ~mStopDrawFlagMask_Base;
}

void Effect::OnBecomeNotActive()
{
	// 基底クラスの同関数を呼ぶ
	Actor::OnBecomeNotActive();

	// メッシュコンポーネントをアクティブにする
	mMeshComp->SetActive(true);
}

void Effect::StopProcess()
{
	// 更新と描画を停止するフラグを設定
	mFlags |= (mStopUpdateFlagMask_Base | mStopDrawFlagMask_Base);
}
