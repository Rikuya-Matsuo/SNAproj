#include "NAFire.h"
#include "System.h"
#include "FireActor.h"

NAFire::NAFire(Player * player):
	NinjaArtsBase(player)
{
	mIconTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/NAFireUI.png");

	mFire = new FireActor(player);
}

void NAFire::Use()
{
	mFire->SetActive(true);
	mFire->SetPosition(mUser->GetPosition());
	mFire->SetScale(mUser->GetScale());
}
