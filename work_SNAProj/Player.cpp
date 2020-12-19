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
#include "NAReelString.h"
#include "NAFire.h"
#include "NinjaArtsUICircle.h"

#ifdef DEBUG_SNA
//#define IMMORTAL_PLAYER
#endif

const char Player::mLifeMax = 10;
const char Player::mDashAttackPower = 1;

const Player::FlagType Player::mDetectWallFlagMask				= 1 << 0;
const Player::FlagType Player::mDetectGroundFlagMask			= 1 << 1;
const Player::FlagType Player::mLookRightFlagMask				= 1 << 2;
const Player::FlagType Player::mImmortalFlagMask				= 1 << 3;
const Player::FlagType Player::mAliveFlagMask					= 1 << 4;
const Player::FlagType Player::mKnockBackFlagMask				= 1 << 5;
const Player::FlagType Player::mAllowJumpFlagMask				= 1 << 6;
const Player::FlagType Player::mActiveBrakeFlagMask				= 1 << 7;
const Player::FlagType Player::mSelfControlAnimationFlagMask	= 1 << 8;

const Vector3D Player::mKnockBackVector = Vector3D(20.0f, 0.0f, 8.0f);

Player::Player() :
	Actor(),
	mFlags_Player(mLookRightFlagMask | mAliveFlagMask | mAllowJumpFlagMask | mActiveBrakeFlagMask | mSelfControlAnimationFlagMask),
	mGroundChecker(nullptr),
	mAttackCollider(nullptr),
	mCurrentAnimation(AnimationPattern::Anim_Stay),
	mWallPointer(nullptr),
	mLife(mLifeMax),
	mHitEffectMass(4)
{
	mPushedFlags.Init();

#ifdef IMMORTAL_PLAYER
	mFlags_Player |= mImmortalFlagMask;
#endif
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

	// 地面を検出するコライダーの生成
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
	
	// 壁を検出するコライダーの生成
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

	// 攻撃コライダーの生成
	AABB attackCol = mMesh->GetCollisionBox();
	float bodyColSizeX = bodyCol.mMax.x - bodyCol.mMin.x;
	attackCol.mMin.x += bodyColSizeX;
	attackCol.mMax.x += bodyColSizeX * 0.25f;
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
	mClampSpeedComponent = new ClampSpeedComponent(this, limitSpeed);
	mClampSpeedComponent->SetClampDirectionFlags(true, false, false);

	// 攻撃ヒットエフェクトの配列
	mHitEffects = new AnimationEffect*[mHitEffectMass];
	for (size_t i = 0; i < mHitEffectMass; ++i)
	{
		mHitEffects[i] = new AnimationEffect(mPriority + 50,
			"Assets/hitEff1.png", 4, 2, 2, 256, 256, 0.09f);
		mHitEffects[i]->SetScale(50.0f);
	}

	// 忍術の設定
	NinjaArtsBase * latestSetNA;
	latestSetNA = new NAReelString(this);
	mNAReelStringIndex = static_cast<Uint8>(mNinjaArts.size());
	mNinjaArts.emplace_back(latestSetNA);

	latestSetNA = new NAFire(this);
	mNAFireIndex = static_cast<Uint8>(mNinjaArts.size());
	mNinjaArts.emplace_back(latestSetNA);

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

	for (auto itr = mNinjaArts.begin(); itr != mNinjaArts.end(); ++itr)
	{
		delete (*itr);

		(*itr) = nullptr;
	}
	mNinjaArts.clear();
	std::vector<NinjaArtsBase *>().swap(mNinjaArts);

	SDL_Log("Player is deleted\n");
}

void Player::SetBeyondSceneFlag(bool value)
{
	// 基底クラスの同関数を呼ぶ
	Actor::SetBeyondSceneFlag(value);

	// 補完アクターも同様にシーンをまたぐよう設定
	mCompletionMeshActor->SetBeyondSceneFlag(value);

	for (auto itr = mNinjaArts.begin(); itr != mNinjaArts.end(); ++itr)
	{
		(*itr)->SetBeyondSceneFlag(value);
	}
}

void Player::UpdateActor0()
{
	// （壁走りバグ対策）
	// 地面を検出していて且つ、下からの押し上げがなく且つ、ジャンプ中である場合
	// 検出したブロックは地面ではなかったとして重力を有効化する
	bool detectGroundFlag = mFlags_Player & mDetectGroundFlagMask;
	bool notPushedUpFlag = mPushedFlags.vertical != 1;
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

	// ジャンプ
	bool jumpInput = Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE) || Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_A);
	bool allowJumpFlag = mFlags_Player & mAllowJumpFlagMask;
	if (jumpInput && detectGroundFlag && allowJumpFlag)
	{
		mJumpComponent->Jump();
	}

	// 地面を検出していない間は重力を有効に
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

	// 忍術発動処理
	// クナイ出すやつ
	bool useReelString =
		Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_E) ||
		Input::GetInstance().GetGamePadButtonPressDown(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y);
	NinjaArtsBase * reelString = mNinjaArts[mNAReelStringIndex];
	if (useReelString && !(reelString->IsUsed()))
	{
		reelString->Use();
	}

	// 火遁の術
	bool useFire =
		Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_R) ||
		Input::GetInstance().GetGamePadButtonPressDown(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X);
	NinjaArtsBase * fire = mNinjaArts[mNAFireIndex];
	if (useFire && !(fire->IsUsed()))
	{
		fire->Use();
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
	// ブレーキ
	bool isKnockingBack = (mFlags_Player & mKnockBackFlagMask);
	bool isActiveBrake = (mFlags_Player & mActiveBrakeFlagMask);
	if (!mInputComponent->GetHorizonInputFlag() && !isKnockingBack && isActiveBrake)
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

	// アニメーション更新
	UpdateAnimation();

	// フラグ記録
	mPrevFlags_Player = mFlags_Player;

	// フラグのリセット
	mFlags_Player &= ~mDetectGroundFlagMask;
	mFlags_Player &= ~mDetectWallFlagMask;

	mPushedFlags.Init();

	// 地面ブロック記録のクリア
	mGroundList.clear();

	// 壁ブロック記録のリセット
	mWallPointer = nullptr;
}

void Player::UpdateAnimation()
{
	if (mFlags_Player & mSelfControlAnimationFlagMask)
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

			OnEndAttack();

			mMesh->GetActiveAnimChips(this)->Reset();

			mCurrentAnimation = AnimationPattern::Anim_Stay;

			mMesh->SetAnimIndex(this, mCurrentAnimation);
		}

		// ノックバックアニメーション
		AnimationChips * knockBackAnim = mMesh->GetAnimChips(this, Anim_KnockBack);
		if (mFlags_Player & mKnockBackFlagMask)
		{
			knockBackAnim->StartPlaying();

			mCurrentAnimation = AnimationPattern::Anim_KnockBack;

			if (knockBackAnim->GetLoopEndFlag())
			{
				knockBackAnim->StopPlaying();

				knockBackAnim->SetTextureIndex(5);
			}

			// 攻撃を終了する
			OnEndAttack();
		}
		else
		{
			knockBackAnim->Reset();
		}
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
}

void Player::OnAttackColliderHits(const ColliderComponentBase * opponent)
{
	// アトリビュート取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// エネミーとの判定
	bool isOpponentEnemy = (opponentAtt == ColliderAttribute::ColAtt_Enemy);
	if (isOpponentEnemy)
	{
		// そのエネミーが一回の攻撃で二度目のヒットをしていないかのチェック
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());

		// 一度の攻撃で誰が当たったかをリストアップしたものから検索する
		auto itr = std::find(mHitList.begin(), mHitList.end(), enemy);

		// ヒットしなかった場合
		if (itr == mHitList.end())
		{
			// リストへの追加
			mHitList.emplace_back(enemy);

			// ダメージを与える
			char power = mDashAttackPower;
			enemy->Damage(power);

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
	// アトリビュート取得
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
	// アトリビュート取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// ブロックとの判定
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
	// アトリビュート取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// 対象がブロックである場合
	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		if (mPushedVector.z)
		{
			// 地面に当たった時の処理
			if (mPushedVector.z > 0.0f)
			{
				OnLanding(opponent);
				return;
			}

			// 天井に当たった時の処理
			if (mPushedVector.z < 0.0f)
			{
				OnCollideCeiling(opponent);
				return;
			}
		}
		else
		{
			// 壁に当たった時の処理
			OnBePushedByWall();
			return;
		}
	}

	// 対象がエネミーである場合
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
	// アトリビュート取得
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

void Player::OnBodyTouching(const ColliderComponentBase * opponent)
{
	// アトリビュート取得
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	// 対象がブロックである場合
	bool isOpponentBlock = (opponentAtt == ColliderAttribute::ColAtt_Block);
	if (isOpponentBlock)
	{
		if (mPushedVector.x)
		{
			OnBePushedByWall();
			return;
		}
		else if (mPushedVector.z)
		{
			OnLanding(opponent);
			return;
		}
	}
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

	// 当たっているのが自身の本体のコライダーの場合
	bool isCallerBody = (caller == mBoxCollider);
	if (isCallerBody)
	{
		OnBodyTouching(opponent);
		return;
	}
}

void Player::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{

}

void Player::LinkNinjaArtsUICircle(NinjaArtsUICircle * naUi)
{
	// UI取得
	mNinjaArtsUI = naUi;

	// UIにプレイヤーを登録
	mNinjaArtsUI->SetPlayer(this);

	// アイコンを登録
	for (auto itr = mNinjaArts.begin(); itr != mNinjaArts.end(); ++itr)
	{
		Texture * icon = (*itr)->GetIconTexture();

		char id = mNinjaArtsUI->ResisterTexture(icon);

		(*itr)->SetIconID(id);
	}
}

void Player::OnDetectGround(const ColliderComponentBase * opponent)
{
	// 壁の真下にあるブロックは地面として扱わない
	if (mWallPointer && mWallPointer != opponent)
	{
		auto getPosition = [](const ColliderComponentBase * collider) -> const Vector3D &
		{
			return collider->GetOwner()->GetPosition();
		};
		float wallToOpponentX = getPosition(opponent).x - getPosition(mWallPointer).x;

		if (!wallToOpponentX)
		{
			return;
		}
	}

	// opponentが、検知している壁である場合も、地面として扱わない
	if (mWallPointer == opponent)
	{
		return;
	}

	// フラグ設定
	mFlags_Player |= mDetectGroundFlagMask;

	// ノックバック中なら、ノックバックを終了
	if (mFlags_Player & mKnockBackFlagMask)
	{
		mFlags_Player &= ~mKnockBackFlagMask;
	}

	// 地面を記録
	mGroundList.emplace_back(opponent);

	// 重力無効化
	SetAffectGravityFlag(false);
}

void Player::OnLanding(const ColliderComponentBase * opponent)
{
	// 壁ブロックの真下にあるブロックは地面ブロックとみなさない
	if (mWallPointer)
	{
		auto getPosition = [](const ColliderComponentBase * collider) -> const Vector3D &
		{
			return collider->GetOwner()->GetPosition();
		};
		Vector3D wallToOpponent = getPosition(opponent) - getPosition(mWallPointer);
		
		// 接触対象の位置ベクトルから壁の位置ベクトルを引き、
		// そのx成分が0ならば真下である。
		if (!wallToOpponent.x && mPushedVector.z)
		{
			mFlags_Player &= ~mDetectGroundFlagMask;
			mFixVector -= mPushedVector;
			SetAffectGravityFlag(true);
			return;
		}
	}

	// 既に押し返しなら押し返しを無効化
	bool isPushedUpAlready = mPushedFlags.vertical > 0;
	if (isPushedUpAlready)
	{
		mFixVector -= mPushedVector;
	}
	// まだしていないならフラグを立てる
	else
	{
		mPushedFlags.vertical = 1;
	}
}

void Player::OnCollideCeiling(const ColliderComponentBase * opponent)
{
	// 壁の真上のブロックなら天井として扱わない
	if (mWallPointer)
	{
		auto getPosition = [](const ColliderComponentBase * collider) -> const Vector3D &
		{
			return collider->GetOwner()->GetPosition();
		};
		Vector3D wallToOpponent = getPosition(opponent) - getPosition(mWallPointer);

		if (!wallToOpponent.x && mPushedVector.z)
		{
			mFixVector -= mPushedVector;
			return;
		}
	}

	// 既にブロックから下方向への押し返しを受けている場合は、押し返しを無効化
	if (mPushedFlags.vertical == -1)
	{
		mFixVector -= mPushedVector;
		return;
	}

	// 押し返し方向設定
	mPushedFlags.vertical = -1;

	// 重力有効化
	SetAffectGravityFlag(true);
}

void Player::OnBePushedByWall()
{
	// 横方向に押されていないなら関数終了
	if (!mPushedVector.x)
	{
		return;
	}

	// 押し返し方向の取得
	char dir = (mPushedVector.x < 0.0f) ? -1 : 1;

	// もし既に同じ方向から押し返しを受けていたらこのフレームで受けた力を無効化
	if (mPushedFlags.horizon == dir)
	{
		mFixVector -= mPushedVector;
	}
	// まだ受けていないならフラグを記録
	else
	{
		mPushedFlags.horizon = dir;
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

	// 忍術のキャンセル
	for (auto ninjaArts = mNinjaArts.begin(); ninjaArts != mNinjaArts.end(); ++ninjaArts)
	{
		NinjaArtsBase * nab = (*ninjaArts);

		if (nab->IsUsed())
		{
			nab->OnBeDamaged();
		}
	}
}

void Player::OnLifeRunOut()
{
	// 生存フラグを下す
	mFlags_Player &= ~mAliveFlagMask;
}

void Player::OnEndAttack()
{
	// アニメーション初期化
	mMesh->GetAnimChips(this, Anim_DashAttack)->Reset();
	mCompletionMeshActor->ResetAnimation(AnimationPattern::Anim_DashAttack);

	// 攻撃コライダーを無効化
	mAttackCollider->SetActive(false);

	// ヒットしたエネミーのリストをクリア
	mHitList.clear();
}

void Player::OnUseNinjaArts()
{
}

void Player::OnEndNinjaArts()
{
}

AnimationEffect * Player::FindNonActiveEffect(AnimationEffect ** effArray, size_t mass) const
{
	// アクティブでないエフェクトを指す
	AnimationEffect * ret = nullptr;

	// アクティブでないエフェクトを探す
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
