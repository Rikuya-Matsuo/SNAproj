#include "NAFire.h"
#include "System.h"
#include "FireActor.h"

NAFire::NAFire(Player * player):
	NinjaArtsBase(player)
{
	// UI�A�C�R���e�N�X�`���̃��[�h
	mIconTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/NAFireUI.png");

	// �΂̃A�N�^�[������
	mFire = new FireActor(player);
}

void NAFire::Use()
{
	// �΂̃A�N�e�B�u���E�ʒu�ƃX�P�[������
	mFire->SetActive(true);
	mFire->SetPosition(mUser->GetPosition());
	mFire->SetScale(mUser->GetScale());
}
