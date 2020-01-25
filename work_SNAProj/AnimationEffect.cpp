#include "AnimationEffect.h"
#include "AnimationChips.h"
#include "Mesh.h"
#include "System.h"

AnimationEffect::AnimationEffect(int priority, const std::string & animPath, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame):
	Effect(nullptr, priority)
{
	mAnimChip = new AnimationChips;
	mMesh->LoadDivTexture(animPath, System::GetInstance().GetRenderer(), this, allNum, xNum, yNum, chipW, chipH, secondPerFrame, 0);
}

AnimationEffect::~AnimationEffect()
{
}
