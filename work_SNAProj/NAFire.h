#pragma once
#include "NinjaArtsBase.h"

class FireActor;

class NAFire : public NinjaArtsBase
{
public:
	NAFire(Player * player);

	void Use() override;

private:
	// �΂̃A�N�^�[
	FireActor * mFire;
};
