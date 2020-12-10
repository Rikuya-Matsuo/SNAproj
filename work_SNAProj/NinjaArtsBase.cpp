#include "NinjaArtsBase.h"
#include "Mesh.h"
#include "CompletionMeshActor.h"

NinjaArtsBase::NinjaArtsBase(Player * user):
	mUser(user),
	mIconTexture(nullptr),
	mIconID(-1)
{
}

NinjaArtsBase::~NinjaArtsBase()
{
}

void NinjaArtsBase::Use()
{
	// 使用中フラグの設定
	mIsUsedFlag = true;
}

void NinjaArtsBase::EndNinjaArts()
{
	// 使用中フラグの設定
	mIsUsedFlag = false;
}

void NinjaArtsBase::OnBeDamaged()
{
}
