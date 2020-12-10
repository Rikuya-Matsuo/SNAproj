#include "NAFire.h"
#include "System.h"
#include "FireActor.h"

NAFire::NAFire(Player * player):
	NinjaArtsBase(player)
{
	// UIアイコンテクスチャのロード
	mIconTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/NAFireUI.png");

	// 火のアクターを持つ
	mFire = new FireActor(player);
	mFire->SetOwnerNinjaArts(this);
}

void NAFire::Use()
{
	// 火のアクターが非アクティブの時のみ
	if (!mFire->GetActiveFlag())
	{
		// 火のアクティブ化・位置とスケール調整
		mFire->SetActive(true);
		mFire->SetPosition(mUser->GetPosition());
		mFire->SetScale(mUser->GetScale());
	}
}
