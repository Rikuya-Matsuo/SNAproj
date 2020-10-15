#include "NAReelString.h"
#include "ReelStringEdgeActor.h"
#include "InputMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "EnemyBase.h"
#include "System.h"
#include "Texture.h"
#include "Mesh.h"
#include "AnimationChips.h"

NAReelString::NAReelString(Player * user):
	NinjaArtsBase(user),
	mRightDashVector(Vector3D(300.0f, 0.0f, 0.0f)),
	mDashVector(Vector3D::zero),
	mDashSpeed(300.0f)
{
	mEdge = new ReelStringEdgeActor(this);

	mIconTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/NAReelStringUI.png");
}

NAReelString::~NAReelString()
{
}

void NAReelString::Use()
{
	NinjaArtsBase::Use();

	bool lookRight = mUser->GetLookRightFlag();

	mEdge->Launch(lookRight);

	mUser->SetAffectGravityFlag(false);

	SetAllowJumpFlagOfUser(false);

	GetInputMoveComponent()->SetActive(false);

	SetActiveBrakeFlagOfUser(false);

	SetSelfControlAnimationFlag(false);

	Player::AnimationPattern dashAtkAnim = Player::AnimationPattern::Anim_DashAttack;
	SetAnimationIndex(dashAtkAnim);
	AnimationChips * anim = GetMesh()->GetAnimChips(mUser, dashAtkAnim);
	anim->Reset();
	anim->StartPlaying();

	mUserSpeedLimitRecord = GetClampSpeedComponent()->GetLimit();

	mUser->SetMoveVector(Vector3D::zero);
}

void NAReelString::CancelNinjaArts()
{
	mEdge->Cancel();
}

void NAReelString::TellEndNinjaArts()
{
	mUser->SetAffectGravityFlag(true);

	SetAllowJumpFlagOfUser(true);

	GetInputMoveComponent()->SetActive(true);

	GetClampSpeedComponent()->SetLimit(mUserSpeedLimitRecord);

	SetActiveBrakeFlagOfUser(true);

	SetSelfControlAnimationFlag(true);

	AnimationChips* atkAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_DashAttack);
	atkAnim->StartPlaying();

	AnimationChips * runAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_Run);
	runAnim->StartPlaying();

	mIsUsedFlag = false;
}

void NAReelString::TellRunningNinjaArts()
{
	mUser->SetAffectGravityFlag(false);

	AnimationChips * currentAnim = GetMesh()->GetActiveAnimChips(mUser);
	AnimationChips * atkAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_DashAttack);
	
	// 発動時の剣を振るモーションを、振り切ったままで維持
	if (atkAnim->GetCurrentTextureIndex() == atkAnim->GetFrameMass() - 2 && currentAnim == atkAnim)
	{
		atkAnim->StopPlaying();
	}

	Vector3D v = Vector3D::zero;
	if (mEdge->GetReelState() == ReelStringEdgeActor::ReelState::ReelState_Block)
	{
		v = mDashVector;

		// キャラチップ設定
		Player::AnimationPattern runAnim = Player::AnimationPattern::Anim_Run;
		AnimationChips * anim = GetMesh()->GetAnimChips(mUser, runAnim);
		SetAnimationIndex(runAnim);
		GetMesh()->SetAnimIndex(mUser, runAnim);
		anim->SetTextureIndex(0);
		anim->StopPlaying();
	}
	mUser->SetMoveVector(v);


	if (mEdge->GetReelState() == ReelStringEdgeActor::ReelState::ReelState_Enemy)
	{
		BringEnemy();
	}
}

void NAReelString::CalculateDashVector()
{
	char dir = (mEdge->GetPosition().x > mUser->GetPosition().x) ? 1 : -1;

	mDashVector = mRightDashVector;
	mDashVector.x *= dir;

	GetClampSpeedComponent()->SetLimit(mDashVector);

	Actor * hitBlock = mEdge->GetHitBlock();
	float diffHeight = hitBlock->GetPosition().z - mUser->GetPosition().z;
	if (diffHeight > 0.0f)
	{
		float distance = hitBlock->GetPosition().x - mUser->GetPosition().x;
		mDashVector.z += diffHeight / distance;
	}
}

void NAReelString::BringEnemy()
{
	Actor * enemy = mEdge->GetHitEnemy();

	const Vector3D & v = mEdge->GetMoveVector();
	enemy->SetMoveVector(v);
}
