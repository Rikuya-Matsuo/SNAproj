#pragma once
#include "Actor.h"

class Player;

class FireActor : public Actor
{
public:
	FireActor(Player * user);
	~FireActor();

private:
	// ���݂��Ă����鎞��(s)
	static const float mTimeLimit;

	// �^�C�}�[
	float mTimer;

	// �g�p��
	Player * mUser;

	void UpdateActor0() override;

	void OnBecomeActive() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
