﻿#pragma once
#include "NinjaArtsBase.h"
#include "Vector.h"

class ColliderComponentBase;
class ReelStringEdgeActor;

class NAReelString : public NinjaArtsBase
{
public:
	NAReelString(Player * user);
	~NAReelString();

	void Use() override;

	void CancelNinjaArts() override;

	friend ReelStringEdgeActor;

private:
	ReelStringEdgeActor * mEdge;

	const Vector3D mRightDashVector;

	const float mDashSpeed;

	Vector3D mDashVector;

	Vector3D mUserSpeedLimitRecord;

	////////////////////////////////////////////////
	// ReelStringEdgeActor（以下、mEdge）専用
	////////////////////////////////////////////////

	// 忍術の終了を教える
	void TellEndNinjaArts();

	// 忍術がまだ発動中であることを教える
	void TellRunningNinjaArts();

	// mEdgeに向かってプレイヤーを動かす際の方向の計算
	void CalculateDashVector();

	void BringEnemy();
};
