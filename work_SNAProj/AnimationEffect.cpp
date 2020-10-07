#include "AnimationEffect.h"
#include "AnimationChips.h"
#include "Mesh.h"
#include "System.h"

AnimationEffect::AnimationEffect(int priority, const std::string & animPath, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame):
	Effect(nullptr, priority),
	mAnimChip(nullptr)
{
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
	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}

void AnimationEffect::UpdateActor1()
{
	if (mAnimChip->GetLoopEndFlag())
	{
		StopProcess();

		mAnimChip->Reset();
	}
}

void AnimationEffect::OnBecomeActive()
{
	Effect::OnBecomeActive();

	mAnimChip->Reset();
}
