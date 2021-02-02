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
#include "AnimationEffect.h"

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
	mFindEffectOffset(Vector3D(0.5f, 0.0f, 0.5f))
{
	// メッシュのロード・設定
	mMesh->LoadDivTexture("Assets/knight0.png", System::GetInstance().GetRenderer(), this, 6, 3, 2, 60, 60, 0.3f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	// アニメーションチップの描画順を設定
	int routine[] = { 3,4,5,-1 };
	mAnimChips->SetRoutine(routine);

	// 自動移動コンポーネントの生成・設定
	mAutoMoveComp = new AutoMoveComponent(this, mVelocity);
	mAutoMoveComp->SetReverseFlag(true, false, false);

	// スピード制限コンポーネントの生成・設定
	mClamper = new ClampSpeedComponent(this, mNormalVelocityLimit);
	mClamper->SetClampDirectionFlags(true, false, false);

	// コリジョン設定
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

	// 突進時のエフェクト
	mDashEffect = new AnimationEffect(mPriority + 50, "Assets/EnemyDashEff4Fr.png", 4, 4, 1, 64, 64, 0.1f);
	mDashEffect->SetScale(mScale);
	mDashEffect->SetEndlessLoopFlag(true);

	// ステージデータを参照した当たり判定を行うコンポーネント
	mBlockChecker = new BlockHitChecker(this, mBodyCollision);

	// 前フレームのフラグ状態変数を初期化
	mPrevFlags_EnemyWalker = mFlags_EnemyWalker;

	// 落下速度の設定
	mFallSpeedRate = 10.0f;

	// 重力を一時無効化
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
		Vector3D findEffOffset = mFindEffectOffset * mScale;
		findEffOffset.x *= (mFlags_Enemy & mLookRightFlagMask_EBase) ? -1.0f : 1.0f;
		Vector3D findEffPos = mPosition + findEffOffset;
		mFindPlayerEffect->SetPosition(findEffPos);

		Vector3D dashEffPos = mPosition;
		mDashEffect->SetPosition(dashEffPos);
		mDashEffect->SetRotation(mRotation);
	}

	// 画面外にいる間はプレイヤー検知装置を非アクティブに
	// ただし、忍術に捕まっていないときに限る
	if (!(mFlags_Enemy & mBeCapturedFlagMask_EBase))
	{
		bool isInCamera = GetInCameraFlag();

		mPlayerDetector->SetActive(isInCamera);
	}
	
	// ブロックにヒットした方向の取得
	const Uint8 blhit = mBlockChecker->GetHitDirectionFlags();

	// 左右の足元にブロックがあるかのフラグの設定
	mFlags_EnemyWalker |= blhit & mRDVerMask ? mRDetectGroundFlagMask : 0;
	mFlags_EnemyWalker |= blhit & mLDVerMask ? mLDetectGroundFlagMask : 0;
	
	// 足元のブロックフラグだけを抽出した値
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
		if (mPrevFlags_EnemyWalker & mFlags_EnemyWalker & mKnockBackFlagMask)
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
			// 速度の設定
			mAutoMoveComp->SetVelocity(mVelocity);
			if (mFlags_Enemy & mLookRightFlagMask_EBase)
			{
				mAutoMoveComp->ReverseVelocity();
			}

			// 制限値の設定
			mClamper->SetLimit(mNormalVelocityLimit);

			// タックルに関するフラグを偽に
			mFlags_EnemyWalker &= ~mTackleFlagMask;
			mFlags_Enemy &= ~mAttackFlagMask_EBase;

			// エフェクト無効化
			mDashEffect->SetActive(false);
		}

		// タイマー初期化
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
	// タックル待機時間中
	if (mTackleWaitTimer < mTackleWait)
	{
		// タイマー計算
		mTackleWaitTimer += System::GetInstance().GetDeltaTime();

		// 自動移動を切る
		mAutoMoveComp->SetActive(false);

		// プレイヤー発見エフェクトを有効化
		mFindPlayerEffect->SetActive(true);
	}
	else
	{
		// 突進開始
		mFlags_EnemyWalker |= mTackleFlagMask;
		mFlags_Enemy |= mAttackFlagMask_EBase;

		// このフレームでタックルを始めた場合
		if (!(mPrevFlags_EnemyWalker & mTackleFlagMask))
		{
			// 自動移動を有効化・速度の設定
			mAutoMoveComp->SetActive(true);
			float f = (mPlayerDirection.x > 0.0f) ? -1.0f : 1.0f;
			mAutoMoveComp->SetVelocity(mTackleVelocity * f);

			// 速度制限をタックル用に設定
			mClamper->SetLimit(mTackleVelocityLimit);

			// プレイヤー発見エフェクトを無効化
			mFindPlayerEffect->SetActive(false);

			// 突進エフェクトを有効化
			mDashEffect->SetActive(true);
		}
	}
}

void EnemyWalker::OnBePushedByPlayer(const ColliderComponentBase * caller, Uint8 oppAtt)
{
	// 本体のコリジョンではない or 対象がプレイヤーではない場合、関数終了
	if (caller != mBodyCollision || oppAtt != ColliderAttribute::ColAtt_Player)
	{
		return;
	}

	// 押されたのではなく、踏まれたのであれば別の関数を呼び、この関数を終了する
	if (mPushedVector.z < 0.0f)
	{
		OnBePressedByPlayer();
		return;
	}

	// 横方向に押された場合
	if (mPushedVector.x)
	{
		// ネームスペースを一時的に省略
		namespace hitDirMask = BlockHitDirectionFlagMask;

		// 接触したブロックの方向を取得
		Uint8 hitBlockDir = mBlockChecker->GetHitDirectionFlags();

		// 押されたベクトルを無効化するケースに当たるかを検証する
		bool pushInvalidateFlag = false;
		
		// 右に押されており、右にブロックがある or 左に押されており、左にブロックがある場合
		// 押されたベクトルを無効化する
		if (mPushedVector.x > 0.0f)
		{
			pushInvalidateFlag = (hitBlockDir & hitDirMask::mRightMask);
		}
		else
		{
			pushInvalidateFlag = (hitBlockDir & hitDirMask::mLeftMask);
		}

		// フラグに応じて押し返しベクトルを無効化
		if (pushInvalidateFlag)
		{
			mFixVector -= mPushedVector;
		}
	}
}

void EnemyWalker::OnBePressedByPlayer()
{
	// 縦方向の押し返しを無効化
	mFixVector.z -= mPushedVector.z;
	mPushedVector.z = 0;
}

void EnemyWalker::OnLifeRunOut()
{
	// 基底クラスの同関数を呼ぶ
	EnemyBase::OnLifeRunOut();

	// 突進エフェクトの無効化
	mDashEffect->SetActive(false);
}

void EnemyWalker::UpdateEnemy1()
{
	// ノックバック処理
	if (mFlags_EnemyWalker & mKnockBackFlagMask)
	{
		// このフレームからノックバックを開始した場合移動ベクトルをノックバックに設定する
		if (!(mPrevFlags_EnemyWalker & mKnockBackFlagMask))
		{
			mMoveVector = mKnockBackVector;
		}
		mMoveVector.x = mKnockBackVector.x;

		// 自動移動を無効化
		mAutoMoveComp->SetActive(false);
	}

	// ノックバック終了時の処理
	if (mPrevFlags_EnemyWalker & mKnockBackFlagMask && !(mFlags_EnemyWalker & mKnockBackFlagMask))
	{
		// 速度制限の設定
		Vector3D lim = (mFlags_EnemyWalker & mTackleFlagMask) ? mTackleVelocityLimit : mNormalVelocityLimit;
		mClamper->SetLimit(lim);
		mClamper->SetClampDirectionFlags(true, false, false);
	}

	// 着地しているかの判定・処理
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

	// フラグ状態記録
	mPrevFlags_EnemyWalker = mFlags_EnemyWalker;

	// 当たり判定関連のフラグを偽にする
	EnemyWalker::FlagType mask =
		(mRDetectGroundFlagMask | mLDetectGroundFlagMask | mHitWallFlagMask | mDetectPlayerFlagMask | mDetectWallFlagMask);
	mFlags_EnemyWalker &= ~mask;
}

void EnemyWalker::OnFlip()
{
	// 右を向いているかのフラグ
	bool lookRight = mFlags_Enemy & mLookRightFlagMask_EBase;

	// フラグ反転
	BitFlagFunc::SetFlagByBool(!lookRight, mFlags_Enemy, mLookRightFlagMask_EBase);

	// 自動移動の向きを反転
	mAutoMoveComp->ReverseVelocity();
}

void EnemyWalker::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// アトリビュートの取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// プレイヤーに押された処理
	OnBePushedByPlayer(caller, opponentAtt);

	// タックル中の処理
	if (mFlags_EnemyWalker & mTackleFlagMask && opponentAtt == ColAtt_Player)
	{
		Player * p = static_cast<Player*>(opponent->GetOwner());
		p->Damage(1);
	}

	// 被攻撃時の処理
	bool isAttacked = (opponentAtt == ColliderAttribute::ColAtt_PlayerAttack || opponentAtt == ColliderAttribute::ColAtt_Fire);
	if (caller == mBodyCollision && isAttacked)
	{
		// ダメージアニメーションフラグ設定
		mFlags_EnemyWalker |= mDamageAnimFlagMask;
		
		// ノックバックフラグ設定
		mFlags_EnemyWalker |= mKnockBackFlagMask;
		// ノックバック方向設定
		float x = opponent->GetOwner()->GetPosition().x - mPosition.x;
		mKnockBackVector = mKnockBackRightVector;
		mKnockBackVector.x *= (x < 0.0f) ? 1.0f : -1.0f;
		// 速度制限設定
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

	// プレイヤー検知処理
	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;
	}

	// 壁検知処理
	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}

void EnemyWalker::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// アトリビュートの取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// プレイヤーに押された時の処理
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

	// プレイヤー検知処理
	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;
	}

	// 壁検知処理
	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}

void EnemyWalker::Capture()
{
	// 基底クラスの同関数を呼ぶ
	EnemyBase::Capture();

	// プレイヤー検知コリジョンの無効化
	mPlayerDetector->SetActive(false);

	// タックルフラグの設定
	BitFlagFunc::SetFlagByBool(false, mFlags_EnemyWalker, mTackleFlagMask);

	// 速度制限の無効化
	mClamper->SetActive(false);

	// 自動移動の無効化
	mAutoMoveComp->SetActive(false);
}

void EnemyWalker::LetGo()
{
	// 基底クラスの同関数を呼ぶ
	EnemyBase::LetGo();

	// プレイヤー検知コリジョンの有効化
	mPlayerDetector->SetActive(true);

	// タックルフラグを有効化
	BitFlagFunc::SetFlagByBool(true, mFlags_EnemyWalker, mTackleFlagMask);

	// 速度制限有効化
	mClamper->SetActive(true);

	// 自動移動有効化
	mAutoMoveComp->SetActive(true);
}
