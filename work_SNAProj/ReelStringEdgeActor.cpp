#include "ReelStringEdgeActor.h"
#include "NAReelString.h"
#include "BoxColliderComponent.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "MeshComponent.h"
#include "Player.h"
#include "EnemyBase.h"
#include "System.h"
#include <cmath>

ReelStringEdgeActor::ReelStringEdgeActor(NAReelString * ninjaArts):
	Actor(),
	mNinjaArts(ninjaArts),
	mHitEnemy(nullptr),
	mAutoMoveVector(Vector3D(10.0f, 0.0f, 0.0f)),
	mLaunchedXDirection(0),
	mDistance(Vector3D::zero),
	mDistanceMaxSq(powf(100.0f, 2)),
	mReelState(ReelState::ReelState_Invalid)
{
	mCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);

	AABB box;
	box.mMin = Vector3D(-0.5f, -0.5f, -0.5f);
	box.mMax = Vector3D(0.5f, 0.5f, 0.5f);

	mCollider->SetObjectBox(box);
	mCollider->SetActive(false);

	mAutoMoveComp = new AutoMoveComponent(this);
	mAutoMoveComp->SetReverseFlag(true, true, true);

	ClampSpeedComponent * csc = new ClampSpeedComponent(this, Vector3D(300.0f, 0.0f, 0.0f));

	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);

	MeshComponent * mc = new MeshComponent(this, 500, false);
	mc->SetMesh(mesh);

	SetAffectGravityFlag(false);

	SetActive(false);

	SetVisible(false);

	mScale = 5.0f;

	SetPriority(mNinjaArts->GetUserPlayer()->GetPriority() + 100);
}

ReelStringEdgeActor::~ReelStringEdgeActor()
{
}

void ReelStringEdgeActor::UpdateActor1()
{
	Actor::UpdateActor1();

	// 距離の記録
	mDistance += mMoveVector;

	// 現在の進行方向算出
	float velX = mAutoMoveComp->GetVelocity().x;
	char dir = (velX > 0) ? 1 : -1;

	// 一定距離まで行ったら戻る
	// この判定は現在の進行方向とフラグの示す方向が一致しているときのみ行う
	if (mLaunchedXDirection == dir && mDistance.LengthSq() >= mDistanceMaxSq)
	{
		mAutoMoveComp->ReverseVelocity();
	}

	mNinjaArts->TellRunningNinjaArts();
}

void ReelStringEdgeActor::OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
	ColliderAttribute oppAtt = opponent->GetColliderAttribute();

	if (oppAtt == ColliderAttribute::ColAtt_Enemy)
	{
		mHitEnemy = dynamic_cast<EnemyBase *>(opponent->GetOwner());

		mReelState = ReelState::ReelState_Enemy;
	}
	else if (oppAtt == ColliderAttribute::ColAtt_Block)
	{
		mAutoMoveComp->SetVelocity(Vector3D::zero);

		mMoveVector = Vector3D::zero;

		mNinjaArts->CalculateDashVector();

		mCollider->SetActive(false);

		mLaunchedXDirection = 0;

		mReelState = ReelState::ReelState_Block;
	}
	else if (oppAtt == ColliderAttribute::ColAtt_Player)
	{
		// 戻ってる状態か、ブロックにヒットしている状態なら非アクティブに
		if (mAutoMoveComp->IsNowReverse() || mReelState == ReelState::ReelState_Block)
		{
			SetActive(false);

			// 忍術クラスに術が終了したことを教える。
			mNinjaArts->TellEndNinjaArts();
		}
	}
}

void ReelStringEdgeActor::Launch(bool lookRight)
{
	mPosition = mNinjaArts->GetUserPlayer()->GetPosition();

	mDistance = mMoveVector = Vector3D::zero;

	mHitEnemy = nullptr;

	mReelState = ReelState::ReelState_Invalid;

	mLaunchedXDirection = (lookRight) ? 1 : -1;

	Vector3D v = mAutoMoveVector;
	v.x *= mLaunchedXDirection;

	mAutoMoveComp->SetVelocity(v);

	mCollider->SetActive(true);

	SetActive(true);

	SetVisible(true);
}
