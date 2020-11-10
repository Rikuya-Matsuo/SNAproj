#include "AnimationEffect.h"
#include "AnimationChips.h"
#include "Mesh.h"
#include "System.h"

AnimationEffect::AnimationEffect(int priority, const std::string & animPath, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame):
	Effect(nullptr, priority),
	mAnimChip(nullptr)
{
	// アニメーションチップのロード
	bool success = mMesh->LoadDivTexture(animPath, System::GetInstance().GetRenderer(), this, allNum, xNum, yNum, chipW, chipH, secondPerFrame, 0);
	if (success)
	{
		mAnimChip = mMesh->GetAnimChips(this, 0);
	}
}

AnimationEffect::~AnimationEffect()
{
}

void AnimationEffect::UpdateActor0()
{
	// 万が一奥に押し返しが発生した場合、エフェクト発生位置の奥行きに設定しなおす
	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}

void AnimationEffect::UpdateActor1()
{
	// アニメーションチップを終端まで表示した際、アニメーションの進行を止めて初期化する
	if (mAnimChip->GetLoopEndFlag())
	{
		StopProcess();

		mAnimChip->Reset();
	}
}

void AnimationEffect::OnBecomeActive()
{
	// エフェクト同様のアクティブ時処理
	Effect::OnBecomeActive();

	// アニメーションチップの初期化
	mAnimChip->Reset();
}
