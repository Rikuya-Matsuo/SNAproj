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
	mIsUsedFlag = true;
}

void NinjaArtsBase::CancelNinjaArts()
{
	mIsUsedFlag = false;
}
