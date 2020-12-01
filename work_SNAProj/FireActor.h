#pragma once
#include "Actor.h"

class Player;

class FireActor : public Actor
{
public:
	FireActor(Player * user);
	~FireActor();

private:
	// 存在していられる時間(s)
	static const float mTimeLimit;

	// タイマー
	float mTimer;

	// 使用者
	Player * mUser;

	void UpdateActor0() override;

	void OnBecomeActive() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
