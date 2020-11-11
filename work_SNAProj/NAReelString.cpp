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
	// Edgeのアクターを生成
	mEdge = new ReelStringEdgeActor(this);

	// アイコン画像読み込み
	mIconTexture = System::GetInstance().GetRenderer()->GetTexture("Assets/NAReelStringUI.png");
}

NAReelString::~NAReelString()
{
}

void NAReelString::Use()
{
	// 基底クラスの同関数を呼ぶ
	NinjaArtsBase::Use();

	// 右を向いているかのフラグを取得
	bool lookRight = mUser->GetLookRightFlag();

	// Edgeを撃ち出す
	mEdge->Launch(lookRight);

	// プレイヤーの重力を無効化
	mUser->SetAffectGravityFlag(false);

	// プレイヤーのジャンプを封じる
	SetAllowJumpFlagOfUser(false);

	// プレイヤーの移動操作を封じる
	GetInputMoveComponent()->SetActive(false);

	// プレイヤーのブレーキを無効化する
	SetActiveBrakeFlagOfUser(false);

	// プレイヤー自身によるアニメーション制御を無効化
	SetSelfControlAnimationFlag(false);

	// 攻撃アニメーション再生開始
	Player::AnimationPattern dashAtkAnim = Player::AnimationPattern::Anim_DashAttack;
	SetAnimationIndex(dashAtkAnim);
	AnimationChips * anim = GetMesh()->GetAnimChips(mUser, dashAtkAnim);
	anim->Reset();
	anim->StartPlaying();

	// 元々のスピード制限を記録
	mUserSpeedLimitRecord = GetClampSpeedComponent()->GetLimit();

	// 移動ベクトルを0に
	mUser->SetMoveVector(Vector3D::zero);
}

void NAReelString::CancelNinjaArts()
{
	// Edgeのキャンセル関数を呼ぶ
	mEdge->Cancel();
}

void NAReelString::TellEndNinjaArts()
{
	// プレイヤーの重力を有効化
	mUser->SetAffectGravityFlag(true);

	// プレイヤーのジャンプ操作を有効化
	SetAllowJumpFlagOfUser(true);

	// プレイヤー移動操作の有効化
	GetInputMoveComponent()->SetActive(true);

	// プレイヤーのスピード制限値を元に戻す
	GetClampSpeedComponent()->SetLimit(mUserSpeedLimitRecord);

	// プレイヤーのブレーキ有効化
	SetActiveBrakeFlagOfUser(true);

	// プレイヤー自身のアニメーション制御を有効化
	SetSelfControlAnimationFlag(true);

	// 停止させていたアニメーションを再生する
	AnimationChips* atkAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_DashAttack);
	atkAnim->StartPlaying();

	AnimationChips * runAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_Run);
	runAnim->StartPlaying();

	// 使用中フラグを偽に
	mIsUsedFlag = false;
}

void NAReelString::TellRunningNinjaArts()
{
	// 重力の不適用を継続
	mUser->SetAffectGravityFlag(false);

	AnimationChips * currentAnim = GetMesh()->GetActiveAnimChips(mUser);
	AnimationChips * atkAnim = GetMesh()->GetAnimChips(mUser, Player::AnimationPattern::Anim_DashAttack);
	
	// 発動時の剣を振るモーションを、振り切ったままで維持
	if (atkAnim->GetCurrentTextureIndex() == atkAnim->GetFrameMass() - 2 && currentAnim == atkAnim)
	{
		atkAnim->StopPlaying();
	}

	// プレイヤー移動速度の計算
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

	// Edgeがエネミーに接触した場合、エネミーを連れてくる
	if (mEdge->GetReelState() == ReelStringEdgeActor::ReelState::ReelState_Enemy)
	{
		BringEnemy();
	}
}

void NAReelString::CalculateDashVector()
{
	// 水平方向の計算
	char dir = (mEdge->GetPosition().x > mUser->GetPosition().x) ? 1 : -1;

	mDashVector = mRightDashVector;
	mDashVector.x *= dir;

	// スピードの制限値をダッシュ用に
	GetClampSpeedComponent()->SetLimit(mDashVector);

	// 移動ベクトル計算
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

	// 捕まえたエネミーを移動させる
	const Vector3D & v = mEdge->GetMoveVector();
	enemy->SetMoveVector(v);
}
