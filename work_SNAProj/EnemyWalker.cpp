#include "EnemyWalker.h"
#include "Mesh.h"
#include "System.h"
#include "AnimationChips.h"
#include "MeshComponent.h"
#include "AutoMoveComponent.h"
#include "BoxColliderComponent.h"
#include "ClampSpeedComponent.h"
#include "BlockHitChecker.h"
#include "Input.h"
#include "Player.h"
#include "Effect.h"

const EnemyWalker::FlagType EnemyWalker::mDamageAnimFlagMask	= 1 << 0;
const EnemyWalker::FlagType EnemyWalker::mLDetectGroundFlagMask = 1 << 1;
const EnemyWalker::FlagType EnemyWalker::mRDetectGroundFlagMask = 1 << 2;
const EnemyWalker::FlagType EnemyWalker::mHitWallFlagMask		= 1 << 3;
const EnemyWalker::FlagType EnemyWalker::mDetectPlayerFlagMask	= 1 << 4;
const EnemyWalker::FlagType EnemyWalker::mDetectWallFlagMask	= 1 << 5;
const EnemyWalker::FlagType EnemyWalker::mTackleFlagMask		= 1 << 6;
const EnemyWalker::FlagType EnemyWalker::mKnockBackFlagMask		= 1 << 7;

const float EnemyWalker::mTackleWait = 0.8f;

using namespace BlockHitDirectionFlagMask;

EnemyWalker::EnemyWalker():
	EnemyBase(3),
	mFlags_EnemyWalker(0),
	mTextureIndex(0),
	mDamageAnimTimer(0.0f),
	mTackleWaitTimer(0.0f),
	mPlayerDirection(Vector3D::zero),
	mWallDerection(Vector3D::zero),
	mVelocity(Vector3D(-10.0f, 0.0f, 0.0f)),
	mTackleVelocity(Vector3D(-20.0f, 0.0f, 0.0f)),
	mNormalVelocityLimit(Vector3D(30.0f, 0.0f, 30.0f)),
	mTackleVelocityLimit(Vector3D(70.0f, 0.0f, 50.0f)),
	mKnockBackRightVector(Vector3D(10.0f, 0.0, 10.0f)),
	mKnockBackVecLimit(Vector3D(50.0f, 0.0f, 50.0f)),
	mEffectOffset(Vector3D(0.5f, 0.0f, 0.5f))
{
	//mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.0f, 0);
	mMesh->LoadDivTexture("Assets/knight0.png", System::GetInstance().GetRenderer(), this, 6, 3, 2, 60, 60, 0.3f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	int routine[] = { 3,4,5,-1 };
	mAnimChips->SetRoutine(routine);
	//mAnimChips->StopPlaying();

	mAutoMoveComp = new AutoMoveComponent(this, mVelocity);
	mAutoMoveComp->SetReverseFlag(true, false, false);

	mClamper = new ClampSpeedComponent(this, mNormalVelocityLimit);
	mClamper->SetClampDirectionFlags(true, false, false);

	const AABB originalBox = mMesh->GetCollisionBox();;
	AABB colBox = mMesh->GetCollisionBox();
	colBox.mMin.y -= EnemyBase::mDepth;
	colBox.mMax.y += EnemyBase::mDepth;
	mBodyCollision = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	mBodyCollision->SetObjectBox(colBox);

	const Vector3D boxSize = originalBox.mMax - originalBox.mMin;
	
	// プレイヤー検出装置
	AABB playerDetector = originalBox;
	playerDetector.mMax.x = playerDetector.mMin.x;
	playerDetector.mMin.x -= boxSize.x * 2.5f;
	playerDetector.mMax.z -= boxSize.z / 5;
	playerDetector.mMin.z += boxSize.z / 5;
	playerDetector.mMin.y -= EnemyBase::mDepth;
	playerDetector.mMax.y += EnemyBase::mDepth;
	mPlayerDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mPlayerDetector->SetObjectBox(playerDetector);

	// プレイヤー発見エフェクト
	mFindPlayerEffect = new Effect("Assets/findPlayerEff.png", mPriority + 50);
	mFindPlayerEffect->SetScale(16.0f);
	mFindPlayerEffect->SetAppearSecond(mTackleWait);

	// スケール
	SetScale(25.0f);

	mBlockChecker = new BlockHitChecker(this, mBodyCollision);

	mPrevFlags_EnemyTest = mFlags_EnemyWalker;

	mFallSpeedRate = 10.0f;

	mFlags &= ~mAffectGravityFlagMask_Base;
}

EnemyWalker::~EnemyWalker()
{
}

void EnemyWalker::UpdateEnemy0()
{
	// エフェクトの出現位置調整
	if (mFlags_EnemyWalker & mDetectPlayerFlagMask)
	{
		Vector3D effOffset = mEffectOffset * mScale;
		effOffset.x *= (mFlags_Enemy & mLookRightFlagMask_EBase) ? -1.0f : 1.0f;
		Vector3D effPos = mPosition + effOffset;
		mFindPlayerEffect->SetPosition(effPos);
	}

	// 画面外にいる間はプレイヤー検知装置を非アクティブに
	// ただし、忍術に捕まっていないときに限る
	if (!(mFlags_Enemy & mBeCapturedFlagMask_EBase))
	{
		mPlayerDetector->SetActive(GetInCameraFlag());
	}
	
	const Uint8 blhit = mBlockChecker->GetHitDirectionFlags();

	mFlags_EnemyWalker |= blhit & mRDVerMask ? mRDetectGroundFlagMask : 0;
	mFlags_EnemyWalker |= blhit & mLDVerMask ? mLDetectGroundFlagMask : 0;
	
	const FlagType detectFlags = mFlags_EnemyWalker & (mLDetectGroundFlagMask | mRDetectGroundFlagMask);
	
	// 左右どちらも床を検出できないかった
	if (!detectFlags)
	{
		// 空中では移動を行わない
		if (mAutoMoveComp->GetActiveFlag())
		{
			mAutoMoveComp->SetActive(false);
		}

		// 重力のアクティブ化（忍術に捕らえられている際は例外）
		bool isCaptured = mFlags_Enemy & mBeCapturedFlagMask_EBase;
		if (!GetAffectGravityFlag() && !isCaptured)
		{
			SetAffectGravityFlag(true);
		}
	}

	// 着地中は重力の影響を受けない(ノックバック例外）
	if (detectFlags != 0 && !(mFlags_EnemyWalker & mKnockBackFlagMask))
	{
		SetAffectGravityFlag(false);
	}

	// 着地時
	if (detectFlags != 0)
	{
		// ノックバックしたばかりでなければフラグを下す
		if (mPrevFlags_EnemyTest & mFlags_EnemyWalker & mKnockBackFlagMask)
		{
			mFlags_EnemyWalker &= ~mKnockBackFlagMask;
		}
	}

	// プレイヤー検知時にタックル準備処理
	if (mFlags_EnemyWalker & mDetectPlayerFlagMask)
	{
		TackleProcess();
	}
	else
	{
		// 着地している　&&　自動移動が非アクティブ
		if (detectFlags && !mAutoMoveComp->GetActiveFlag())
		{
			// 自動移動をアクティブに
			mAutoMoveComp->SetActive(true);
		}

		// タックルしていたなら
		if (mFlags_EnemyWalker & mTackleFlagMask)
		{
			mAutoMoveComp->SetVelocity(mVelocity);
			if (mFlags_Enemy & mLookRightFlagMask_EBase)
			{
				mAutoMoveComp->ReverseVelocity();
			}

			mClamper->SetLimit(mNormalVelocityLimit);

			mFlags_EnemyWalker &= ~mTackleFlagMask;
			mFlags_Enemy &= ~mAttackFlagMask_EBase;
		}

		mTackleWaitTimer = 0.0f;
	}

	// 自動移動が非アクティブなら水平方向移動ベクトルを0に
	// ただし、ノックバックは除く
	// 加えて、忍術に捕縛された場合も除く
	bool exceptionCase = (mFlags_EnemyWalker & mKnockBackFlagMask) || (mFlags_Enemy & mBeCapturedFlagMask_EBase);
	if (!mAutoMoveComp->GetActiveFlag() && !exceptionCase)
	{
		mMoveVector.x = 0.0f;
	}
}

void EnemyWalker::TackleProcess()
{
	if (mTackleWaitTimer < mTackleWait)
	{
		mTackleWaitTimer += System::GetInstance().GetDeltaTime();
		mAutoMoveComp->SetActive(false);

		mFindPlayerEffect->SetActive(true);
	}
	else
	{
		// 突進開始
		mFlags_EnemyWalker |= mTackleFlagMask;
		mFlags_Enemy |= mAttackFlagMask_EBase;

		if (!(mPrevFlags_EnemyTest & mTackleFlagMask))
		{
			mAutoMoveComp->SetActive(true);
			float f = (mPlayerDirection.x > 0.0f) ? -1.0f : 1.0f;
			mAutoMoveComp->SetVelocity(mTackleVelocity * f);

			mClamper->SetLimit(mTackleVelocityLimit);

			mFindPlayerEffect->SetActive(false);
		}
	}
}

void EnemyWalker::OnBePushedByPlayer(const ColliderComponentBase * caller, Uint8 oppAtt)
{
	if (caller != mBodyCollision || oppAtt != ColliderAttribute::ColAtt_Player)
	{
		return;
	}

	if (mPushedVector.z < 0.0f)
	{
		OnBePressedByPlayer();
		return;
	}

	if (mPushedVector.x)
	{
		namespace hitDirMask = BlockHitDirectionFlagMask;
		Uint8 hitBlockDir = mBlockChecker->GetHitDirectionFlags();

		bool pushInvalidateFlag = false;
		if (mPushedVector.x > 0.0f)
		{
			pushInvalidateFlag = (hitBlockDir & hitDirMask::mRightMask);
		}
		else
		{
			pushInvalidateFlag = (hitBlockDir & hitDirMask::mLeftMask);
		}

		if (pushInvalidateFlag)
		{
			mFixVector -= mPushedVector;
		}
	}
}

void EnemyWalker::OnBePressedByPlayer()
{
	mFixVector.z -= mPushedVector.z;
	mPushedVector.z = 0;
}

void EnemyWalker::UpdateEnemy1()
{
	if (mFlags_EnemyWalker & mKnockBackFlagMask)
	{
		if (!(mPrevFlags_EnemyTest & mKnockBackFlagMask))
		{
			mMoveVector = mKnockBackVector;
		}
		mMoveVector.x = mKnockBackVector.x;
		mAutoMoveComp->SetActive(false);
	}

	if (mPrevFlags_EnemyTest & mKnockBackFlagMask && !(mFlags_EnemyWalker & mKnockBackFlagMask))
	{
		Vector3D lim = (mFlags_EnemyWalker & mTackleFlagMask) ? mTackleVelocityLimit : mNormalVelocityLimit;
		mClamper->SetLimit(lim);
		mClamper->SetClampDirectionFlags(true, false, false);
	}

	const Uint8 blhit = mBlockChecker->GetHitDirectionFlags();
	if (blhit & mDownMask && !(mFlags_EnemyWalker & mKnockBackFlagMask))
	{
		mMoveVector.z = 0;
		SetAffectGravityFlag(false);
	}

	// 反転するか否かの判定
	// ブロックにぶち当たった場合、もしくは行く先の足場ブロックがない場合true
	bool flipFlag =
		(mFlags_Enemy & mLookRightFlagMask_EBase) ?
		(blhit & mRightMask) || (!(blhit & mRDVerMask) && blhit & mLDVerMask) :
		(blhit & mLeftMask) || (!(blhit & mLDVerMask) && blhit & mRDVerMask);

	// 反転を行うが、忍術に捕縛されている場合は除く
	if (flipFlag && !(mFlags_Enemy & mBeCapturedFlagMask_EBase))
	{
		Flip();
	}

	mPrevFlags_EnemyTest = mFlags_EnemyWalker;

	EnemyWalker::FlagType mask =
		(mRDetectGroundFlagMask | mLDetectGroundFlagMask | mHitWallFlagMask | mDetectPlayerFlagMask | mDetectWallFlagMask);
	mFlags_EnemyWalker &= ~mask;
}

void EnemyWalker::OnFlip()
{
	bool lookRight = mFlags_Enemy & mLookRightFlagMask_EBase;
	BitFlagFunc::SetFlagByBool(!lookRight, mFlags_Enemy, mLookRightFlagMask_EBase);
	mAutoMoveComp->ReverseVelocity();
}

void EnemyWalker::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	OnBePushedByPlayer(caller, opponentAtt);

	if (mFlags_EnemyWalker & mTackleFlagMask && opponentAtt == ColAtt_Player)
	{
		Player * p = static_cast<Player*>(opponent->GetOwner());
		p->Damage(1);
	}

	if (caller == mBodyCollision && opponentAtt == ColliderAttribute::ColAtt_PlayerAttack)
	{
		mFlags_EnemyWalker |= mDamageAnimFlagMask;
		
		mFlags_EnemyWalker |= mKnockBackFlagMask;
		float x = opponent->GetOwner()->GetPosition().x - mPosition.x;
		mKnockBackVector = mKnockBackRightVector;
		mKnockBackVector.x *= (x < 0.0f) ? 1.0f : -1.0f;
		mClamper->SetLimit(mKnockBackVecLimit);
		mClamper->SetClampDirectionFlags(true, false, true);

		mFixVector.x = 0.0f;
	}

	// 地面検出装置の処理
	auto checkPointer = 
		[this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask, ColliderAttribute cmpAtt = ColliderAttribute::ColAtt_Block)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == cmpAtt)
		{
			mFlags_EnemyWalker |= mask;

			ret = true;
		}

		return ret;
	};

	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;
	}

	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}

void EnemyWalker::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	OnBePushedByPlayer(caller, opponentAtt);

	// 地面検出装置の処理
	auto checkPointer = 
		[this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask, ColliderAttribute cmpAtt = ColliderAttribute::ColAtt_Block)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == cmpAtt)
		{
			mFlags_EnemyWalker |= mask;

			ret = true;
		}

		return ret;
	};

	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;
	}

	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}

void EnemyWalker::Capture()
{
	EnemyBase::Capture();

	mPlayerDetector->SetActive(false);

	BitFlagFunc::SetFlagByBool(false, mFlags_EnemyWalker, mTackleFlagMask);

	mClamper->SetActive(false);

	mAutoMoveComp->SetActive(false);
}

void EnemyWalker::LetGo()
{
	EnemyBase::LetGo();

	mPlayerDetector->SetActive(true);

	BitFlagFunc::SetFlagByBool(true, mFlags_EnemyWalker, mTackleFlagMask);

	mClamper->SetActive(true);

	mAutoMoveComp->SetActive(true);
}
