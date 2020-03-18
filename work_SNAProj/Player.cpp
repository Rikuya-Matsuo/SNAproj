#include "Player.h"
#include "Common.h"
#include "CompletionMeshActor.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"
#include "JumpComponent.h"
#include "ClampSpeedComponent.h"
#include "AnimationChips.h"
#include "Mesh.h"
#include "System.h"
#include "Renderer.h"
#include "Input.h"
#include "EnemyBase.h"
#include "AnimationEffect.h"
#include "NinjaArtsBase.h"

const char Player::mLifeMax = 10;
const char Player::mDashAttackPower = 1;

const Player::FlagType Player::mDetectWallFlagMask		= 1 << 0;
const Player::FlagType Player::mDetectGroundFlagMask	= 1 << 1;
const Player::FlagType Player::mLookRightFlagMask		= 1 << 2;
const Player::FlagType Player::mImmortalFlagMask		= 1 << 3;
const Player::FlagType Player::mAliveFlagMask			= 1 << 4;
const Player::FlagType Player::mKnockBackFlagMask		= 1 << 5;
const Player::FlagType Player::mLandingPushUpFlagMask	= 1 << 6;

const Vector3D Player::mKnockBackVector = Vector3D(20.0f, 0.0f, 8.0f);

Player::Player() :
	Actor(),
	mFlags_Player(mLookRightFlagMask | mAliveFlagMask),
	mGroundChecker(nullptr),
	mAttackCollider(nullptr),
	mCurrentCursorNinjaArts(nullptr),
	mCurrentAnimation(AnimationPattern::Anim_Stay),
	mWallPointer(nullptr),
	mLife(mLifeMax),
	mHitEffectMass(4)
{
	// フラグコピー
	mPrevFlags_Player = mFlags_Player;

	// メッシュのロード
	const int drawOrder = 300;
	const float dashAttackAnimSpeed = 0.05f;
	MeshComponent * mc = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	// 待機アニメーション
	mMesh->LoadDivTexture("Assets/NinjaStay.png", System::GetInstance().GetRenderer(), this,
		10, 10, 1, 128, 128, 0.07f, AnimationPattern::Anim_Stay);
	// 攻撃アニメーション
	mMesh->LoadDivTexture("Assets/NinjaDashAttack02.png", System::GetInstance().GetRenderer(), this,
		8, 8, 1, 128, 128, dashAttackAnimSpeed, AnimationPattern::Anim_DashAttack);
	// 走りアニメーション
	mMesh->LoadDivTexture("Assets/NinjaRun02.png", System::GetInstance().GetRenderer(), this,
		5, 5, 1, 128, 128, 0.05f, AnimationPattern::Anim_Run);
	AnimationChips * runChips = mMesh->GetAnimChips(this, Anim_Run);
	int routine[] = { 0,1,2,3,-1 };
	runChips->SetRoutine(routine);
	// ノックバックアニメーション
	mMesh->LoadDivTexture("Assets/NinjaDown.png", System::GetInstance().GetRenderer(), this,
		6, 6, 1, 128, 128, 0.06f, AnimationPattern::Anim_KnockBack);
	mc->SetMesh(mMesh);

	// チップからはみ出た部分を描画するためのアクター
	mCompletionMeshActor = new CompletionMeshActor(this, drawOrder);
	mCompletionMeshActor->LoadAnimation("Assets/NinjaDashAttack02_completion.png", System::GetInstance().GetRenderer(),
		8, 8, 1, 128, 128, dashAttackAnimSpeed, AnimationPattern::Anim_DashAttack);
	Mesh * completionMesh = mCompletionMeshActor->GetMesh();
	AABB complMeshBox = completionMesh->GetCollisionBox();
	const float offsetX = (complMeshBox.mMax.x - complMeshBox.mMin.x) * mScale;
	Vector3D cmaPosOffset = Vector3D(offsetX, 0, 0);
	mCompletionMeshActor->SetPositionOffset(cmaPosOffset);
	mCompletionMeshActor->SetFlipDirection(true, false);

	// コライダーの設定
	mBoxCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Player);
	AABB bodyCol = mMesh->GetCollisionBox();
	mBoxCollider->SetObjectBox(bodyCol);

	const bool genDetectorFlag = true;
	if (genDetectorFlag)
	{
		AABB box = bodyCol;
		const Vector3D boxSize = bodyCol.mMax - bodyCol.mMin;
		box.mMax.z = box.mMin.z;
		box.mMin.z -= 0.1f;

		float detectorXOffset = 0.15f;
		box.mMax.x -= detectorXOffset;
		box.mMin.x += detectorXOffset;

		mGroundChecker = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
		mGroundChecker->SetObjectBox(box);
	}
	
	{
		AABB box = bodyCol;
		const Vector3D boxSize = bodyCol.mMax - bodyCol.mMin;
		float zOffset = boxSize.z * 0.3f;
		box.mMax.z -= zOffset;
		box.mMin.z += zOffset;

		box.mMin.x = box.mMax.x;
		box.mMax.x += 0.05f;

		mWallChecker = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
		mWallChecker->SetObjectBox(box);
		mWallChecker->SetCheckOrder(50);
	}

	AABB attackCol = mMesh->GetCollisionBox();
	float bodyColSizeX = bodyCol.mMax.x - bodyCol.mMin.x;
	attackCol.mMin.x += bodyColSizeX;
	attackCol.mMax.x += bodyColSizeX;
	mAttackCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_PlayerAttack);
	mAttackCollider->SetObjectBox(attackCol);
	mAttackCollider->SetActive(false);
	mAttackCollider->SetMoveOnHitFlag(false);
	mAttackCollider->SetPushOnHitFlag(false);

	// 矢印キー移動コンポーネント
	mInputComponent = new InputMoveComponent(this);
	mInputComponent->SetVerticalAxis(mInputComponent->AxisEnum_Invalid);

	const float speed = 500.0f;
	mInputComponent->SetHorizontalSpeed(speed);
	mInputComponent->SetVerticalSpeed(-speed);

	// ジャンプ機能
	mJumpComponent = new JumpComponent(this);
	mJumpComponent->SetPower(9.0f);

	// 最大速度を調整
	Vector3D limitSpeed(170.0f, 0.0f, 100.0f);
	ClampSpeedComponent * csc = new ClampSpeedComponent(this, limitSpeed);
	csc->SetClampDirectionFlags(true, false, false);

	// 攻撃ヒットエフェクトの配列
	mHitEffects = new AnimationEffect*[mHitEffectMass];
	for (size_t i = 0; i < mHitEffectMass; ++i)
	{
		mHitEffects[i] = new AnimationEffect(mPriority + 50,
			"Assets/hitEff1.png", 4, 2, 2, 256, 256, 0.09f);
		mHitEffects[i]->SetScale(50.0f);
	}

	// 落下スピード割合の調整
	mFallSpeedRate = 25.0f;

	// 落下スピードの制限値設定
	mFallSpeedMax = 6.0f;
}

Player::~Player()
{
	// インスタンス自体はActorクラスを継承しているため、Systemクラスによってメモリ解放が行われる
	delete[] mHitEffects;
	mHitEffects = nullptr;

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
	// （壁走りバグ対策）
	// 地面を検出していて且つ、下からの押し上げがなく且つ、ジャンプ中である場合
	// 検出したブロックは地面ではなかったとして重力を有効化する
	bool detectGroundFlag = mFlags_Player & mDetectGroundFlagMask;
	bool notPushedUpFlag = !(mFlags_Player & mLandingPushUpFlagMask);
	bool isJumping = mMoveVector.z > 0;
	if (detectGroundFlag && notPushedUpFlag && isJumping)
	{
		mFlags_Player &= ~mDetectGroundFlagMask;
		SetAffectGravityFlag(true);
	}

	// 落下時死亡
	if (mPosition.z < -50.0f)
	{
		mLife = 0;
	}

	// 死亡時関数
	if (mLife <= 0 && !(mFlags_Player & mImmortalFlagMask))
	{
		OnLifeRunOut();
	}

	// 死亡したフレームなら全てのコンポーネントを非アクティブ化
	if (!(mFlags_Player & mAliveFlagMask) && mPrevFlags_Player & mAliveFlagMask)
	{
		SetAllComponentActive(false);
	}

	bool jumpInput = Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE) || Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_A);
	if (mFlags_Player & mDetectGroundFlagMask && jumpInput)
	{
		mJumpComponent->Jump();
	}

	if (!(mFlags_Player & mDetectGroundFlagMask))
	{
		SetAffectGravityFlag(true);
	}

	// ダッシュアタック
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_A) || Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_B))
	{
		mCurrentAnimation = AnimationPattern::Anim_DashAttack;

		mAttackCollider->SetActive(true);
	}

	// 制作展では使わない
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_N))
	{
		if (mCurrentCursorNinjaArts)
		{
			mCurrentCursorNinjaArts->Use();
		}
	}

#ifdef DEBUG_SNA
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_LCTRL))
	{
		SDL_Log("height : %lf", mPosition.z);
	}
#endif // DEBUG_SNA
}

void Player::UpdateActor1()
{
	// 走る
	if (mInputComponent->GetHorizonInputFlag())
	{
		if (mCurrentAnimation != Anim_DashAttack)
		{
			mCurrentAnimation = AnimationPattern::Anim_Run;
		}
	}
	else if (mCurrentAnimation != Anim_DashAttack)
	{
		mCurrentAnimation = Anim_Stay;
	}

	// ダッシュアタック終了判定
	if (mCurrentAnimation == AnimationPattern::Anim_DashAttack && mMesh->GetAnimLoopEndFlag())
	{
		if (!(mCompletionMeshActor->GetMesh()->GetAnimLoopEndFlag()))
		{
			SDL_Log("Completion has not finish\n");
		}

		mMesh->GetActiveAnimChips(this)->Reset();
		mCompletionMeshActor->ResetAnimation(AnimationPattern::Anim_DashAttack);

		mCurrentAnimation = AnimationPattern::Anim_Stay;

		mMesh->SetAnimIndex(this, mCurrentAnimation);

		mAttackCollider->SetActive(false);

		mHitList.clear();
	}

	// ノックバックアニメーション
	AnimationChips * knockBackAnim = mMesh->GetAnimChips(this, Anim_KnockBack);
	if (mFlags_Player & mKnockBackFlagMask)
	{
		mCurrentAnimation = AnimationPattern::Anim_KnockBack;

		if (knockBackAnim->GetLoopEndFlag())
		{
			knockBackAnim->StopPlaying();

			knockBackAnim->SetTextureIndex(5);
		}
	}
	else
	{
		knockBackAnim->Reset();
		knockBackAnim->StartPlaying();
	}

	// チップ補完アクターの設置方向を再設定
	if (mScale != mCompletionMeshActor->GetScale())
	{
		Mesh * completionMesh = mCompletionMeshActor->GetMesh();
		AABB complMeshBox = completionMesh->GetCollisionBox();
		float offsetX = (complMeshBox.mMax.x - complMeshBox.mMin.x) * mScale;
		Vector3D cmaPosOffset = Vector3D(offsetX, 0, 0);
		mCompletionMeshActor->SetPositionOffset(cmaPosOffset);

		if (!(mFlags_Player & mLookRightFlagMask))
		{
			mCompletionMeshActor->FlipPositionOffset();
		}
	}

	// ブレーキ
	if (!mInputComponent->GetHorizonInputFlag() && !(mFlags_Player & mKnockBackFlagMask))
	{
		mMoveVector.x *= 0.05f;

		if (fabsf(mMoveVector.x) < 0.001f)
		{
			mMoveVector.x = 0.0f;
		}
	}

	// 向き変更
	if (mInputComponent->GetRightKey())
	{
		if (!(mFlags_Player & mLookRightFlagMask))
		{
			mRotationAngle = 0.0f;

			mFlags_Player |= mLookRightFlagMask;

			if (mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}
	else if (mInputComponent->GetLeftKey())
	{
		if ((mFlags_Player & mLookRightFlagMask))
		{
			mRotationAngle = static_cast<float>(M_PI);

			mFlags_Player &= ~mLookRightFlagMask;

			if (!mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}

	// 奥行きの情報を常に0に
	mPosition.y = 0.0f;

	// メッシュにアニメーションの変更を伝える
	mMesh->SetAnimIndex(this, mCurrentAnimation);

	// チップ補完アクターにも現在のアニメーションを伝える
	mCompletionMeshActor->SetAnimationIndex(mCurrentAnimation);

	// テクスチャ番号を設定
	if (mCompletionMeshActor->IsResisteredIndex(mCurrentAnimation))
	{
		size_t index = mMesh->GetActiveAnimChips(this)->GetCurrentTextureIndex();
		mCompletionMeshActor->GetMesh()->GetAnimChips(mCompletionMeshActor, mCurrentAnimation)->SetTextureIndex(index);
	}

	// フラグ記録
	mPrevFlags_Player = mFlags_Player;

	// フラグのリセット
	mFlags_Player &= ~mLandingPushUpFlagMask;
	mFlags_Player &= ~mDetectGroundFlagMask;
	mFlags_Player &= ~mDetectWallFlagMask;

	// 地面ブロック記録のクリア
	mGroundList.clear();

	// 壁ブロック記録のリセット
	mWallPointer = nullptr;
}

void Player::OnAttackColliderHits(const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// エネミーとの判定
	bool isOpponentEnemy = (opponentAtt == ColliderAttribute::ColAtt_Enemy);
	if (isOpponentEnemy)
	{
		// そのエネミーが一回の攻撃で二度目のヒットをしていないかのチェック
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());

		// 一度の攻撃で誰が当たったかをリストアップしたものから検索する
		auto itr = std::find(mHitList.begin(), mHitList.end(), enemy);

		// ヒットした場合
		if (itr == mHitList.end())
		{
			// リストへの追加
			mHitList.emplace_back(enemy);

			// ダメージを与える
			enemy->Damage(mDashAttackPower);

			// ヒットエフェクトの発生
			AnimationEffect * hitEffect = FindNonActiveEffect(mHitEffects, mHitEffectMass);
			// 非アクティブなエフェクトインスタンスが見つからなかったら関数終了
			if (!hitEffect)
			{
				return;
			}

			hitEffect->SetActive(true);
			// エフェクトの位置の設定
			const Vector3D& enemyPosition = enemy->GetPosition();
			hitEffect->SetPosition(enemyPosition);
		}
	}
}

void Player::OnGroundCheckerHits(const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// ブロックとの判定
	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		OnDetectGround(opponent);
		return;
	}
}

void Player::OnWallCheckerHits(const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		mFlags_Player |= mDetectWallFlagMask;
		mWallPointer = opponent;
		return;
	}
}

void Player::OnBodyHits(const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		// 地面に当たった時の処理
		if (mPushedVector.z > 0.0f)
		{
			OnLanding(opponent);
			return;
		}

		// 天井に当たった時、重力を有効化
		if (mPushedVector.z < 0.0f)
		{
			SetAffectGravityFlag(true);
			return;
		}
	}

	bool isOpponentEnemy = (opponentAtt == ColliderAttribute::ColAtt_Enemy);
	if (isOpponentEnemy)
	{
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());

		if (enemy->GetAttackFlag())
		{
			OnBeAttacked(enemy);
		}
		return;
	}
}

void Player::OnGroundCheckerTouching(const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// ブロックとの判定
	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		OnDetectGround(opponent);
		return;
	}
}

void Player::OnWallCheckerTouching(const ColliderComponentBase * opponent)
{
	// 処理は同じなので。
	OnWallCheckerHits(opponent);
}

void Player::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// 当たったのが自身の本体のコライダーの場合
	bool isCallerBody = (caller == mBoxCollider);
	if (isCallerBody)
	{
		OnBodyHits(opponent);
		return;
	}

	// 当たったのが自身の地面検出装置の場合
	bool isCallerGroundChecker = (caller == mGroundChecker);
	if (isCallerGroundChecker)
	{
		OnGroundCheckerHits(opponent);
		return;
	}

	// 当たったのが自身の壁検出装置の場合
	bool isCallerWallChecker = (caller == mWallChecker);
	if (isCallerWallChecker)
	{
		OnWallCheckerHits(opponent);
		return;
	}

	// 当たったのが自身の攻撃判定コライダーの場合
	bool isCallerAttackCollider = (caller == mAttackCollider);
	if (isCallerAttackCollider)
	{
		OnAttackColliderHits(opponent);
		return;
	}
}

void Player::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// 当たっているのが自身の地面検出装置の場合
	bool isCallerGroundChecker = (caller == mGroundChecker);
	if (isCallerGroundChecker)
	{
		OnGroundCheckerTouching(opponent);
		return;
	}

	// 当たっているのが自身の壁検出装置の場合
	bool isCallerWallChecker = (caller == mWallChecker);
	if (isCallerWallChecker)
	{
		OnWallCheckerTouching(opponent);
		return;
	}
}

void Player::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{

}

void Player::OnDetectGround(const ColliderComponentBase * opponent)
{
	// 壁の真下にあるブロックは地面として扱わない
	if (mWallPointer && mWallPointer != opponent)
	{
		auto getPosition = [](const ColliderComponentBase * collider)
		{
			return collider->GetOwner()->GetPosition();
		};
		float wallToOpponentX = getPosition(opponent).x - getPosition(mWallPointer).x;

		if (!wallToOpponentX)
		{
			SDL_Log("aaa\n");
			return;
		}
	}

	mFlags_Player |= mDetectGroundFlagMask;

	if (mFlags_Player & mKnockBackFlagMask)
	{
		mFlags_Player &= ~mKnockBackFlagMask;
	}

	mGroundList.emplace_back(opponent);

	SetAffectGravityFlag(false);
}

void Player::OnLanding(const ColliderComponentBase * opponent)
{
	// 壁ブロックの真下にあるブロックは地面ブロックとみなさない
	if (mWallPointer)
	{
		auto getPosition = [](const ColliderComponentBase * collider)
		{
			return collider->GetOwner()->GetPosition();
		};
		Vector3D wallToOpponent = getPosition(opponent) - getPosition(mWallPointer);
		
		// 接触対象の位置ベクトルから壁の位置ベクトルを引き、
		// そのx成分が0ならば真下である。
		if (!wallToOpponent.x && mPushedVector.z)
		{
			SDL_Log("HogeHogeHoge!\n");
			mFlags_Player &= ~mDetectGroundFlagMask;
			mFixVector -= mPushedVector;
			SetActive(true);
			return;
		}
	}

	// 既に押し返しなら押し返しを無効化
	bool isPushedUpAlready = mFlags_Player & mLandingPushUpFlagMask;
	if (isPushedUpAlready)
	{
		mFixVector -= mPushedVector;
	}
	// まだしていないならフラグを立てる
	else
	{
		mFlags_Player |= mLandingPushUpFlagMask;
	}
}

void Player::OnBeAttacked(const EnemyBase * enemy)
{
	// ノックバック方向の設定
	// とりあえず右方向に飛ぶ前提で設定
	mMoveVector = mKnockBackVector;

	// 相手が自身よりも右にいる場合はx方向を反転
	bool isEnemyRight = (enemy->GetPosition().x > mPosition.x);
	if (isEnemyRight)
	{
		mMoveVector.x *= -1.0f;
	}

	// ノックバック中フラグを立てる
	mFlags_Player |= mKnockBackFlagMask;

	// エフェクト発生
	AnimationEffect * effect = FindNonActiveEffect(mHitEffects, mHitEffectMass);
	if (effect)
	{
		effect->SetPosition(mPosition);
		effect->SetActive(true);
	}
}

void Player::OnLifeRunOut()
{
	mFlags_Player &= ~mAliveFlagMask;
}

AnimationEffect * Player::FindNonActiveEffect(AnimationEffect ** effArray, size_t mass) const
{
	AnimationEffect * ret = nullptr;

	for (size_t i = 0; i < mass; ++i)
	{
		if (!effArray[i]->GetActiveFlag())
		{
			ret = effArray[i];
			break;
		}
	}

	return ret;
}
