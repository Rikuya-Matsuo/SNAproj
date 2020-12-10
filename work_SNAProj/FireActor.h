#pragma once
#include "Actor.h"

class Player;
class NinjaArtsBase;

class FireActor : public Actor
{
public:
	FireActor(Player * user);
	~FireActor();

	void SetOwnerNinjaArts(NinjaArtsBase * nab) { mOwnerNinjaArts = nab; }

private:
	// 存在していられる時間(s)
	static const float mTimeLimit;

	// タイマー
	float mTimer;

	// 使用者
	Player * mUser;

	// 管理する忍術インスタンス
	NinjaArtsBase * mOwnerNinjaArts;

	void UpdateActor0() override;

	void OnBecomeActive() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
