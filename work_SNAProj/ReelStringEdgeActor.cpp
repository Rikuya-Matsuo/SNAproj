#include "ReelStringEdgeActor.h"
#include "NAReelString.h"
#include "ReelStringStringActor.h"
#include "BoxColliderComponent.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Player.h"
#include "EnemyBase.h"
#include "System.h"
#include <cmath>

ReelStringEdgeActor::ReelStringEdgeActor(NAReelString * ninjaArts):
	Actor(),
	mNinjaArts(ninjaArts),
	mHitEnemy(nullptr),
	mHitBlock(nullptr),
	mAutoMoveVector(Vector3D(10.0f, 0.0f, 0.0f)),
	mLaunchedXDirection(0),
	mDistance(Vector3D::zero),
	mDistanceMaxSq(powf(100.0f, 2)),
	mReelState(ReelState::ReelState_Invalid)
{
	// コライダー設定
	mCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);

	AABB box;
	box.mMin = Vector3D(-0.5f, -0.5f, -0.1f);
	box.mMax = Vector3D(0.5f, 0.5f, 0.1f);

	mCollider->SetObjectBox(box);

	// 自動移動コンポーネント設定
	mAutoMoveComp = new AutoMoveComponent(this);
	mAutoMoveComp->SetReverseFlag(true, true, true);

	// 速度制御コンポーネント
	ClampSpeedComponent * csc = new ClampSpeedComponent(this, Vector3D(300.0f, 0.0f, 0.0f));

	// メッシュ及びメッシュコンポーネント
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadTexture("Assets/GoldKunai.png", System::GetInstance().GetRenderer(), this);

	MeshComponent * mc = new MeshComponent(this, 500, false);
	mc->SetMesh(mesh);

	// 糸のアクター
	mStringActor = new ReelStringStringActor(mNinjaArts, this);

	// 重力不適用
	SetAffectGravityFlag(false);

	// 非アクティブ
	SetActive(false);

	// スケール値設定
	mScale = 10.0f;

	// 更新順はプレイヤーよりも後に設定
	SetPriority(mNinjaArts->GetUserPlayer()->GetPriority() + 100);

	// 回転軸の設定
	mRotationAxis = Vector3D(0.0f, 0.0f, 1.0f);
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
	// アトリビュート取得
	ColliderAttribute oppAtt = opponent->GetColliderAttribute();

	// 対象がエネミーであるとき
	if (oppAtt == ColliderAttribute::ColAtt_Enemy)
	{
		// もう既にエネミーを捕えているなら無効
		if (mReelState == ReelState::ReelState_Enemy)
		{
			return;
		}
		// すでにブロックにヒットしているなら、理不尽な挙動を避けるため忍術をキャンセル
		else if (mReelState == ReelState::ReelState_Block)
		{
			mHitEnemy = dynamic_cast<EnemyBase*>(opponent->GetOwner());

			mNinjaArts->EndNinjaArts();

			return;
		}

		// 当たったエネミーを記録
		mHitEnemy = dynamic_cast<EnemyBase *>(opponent->GetOwner());

		// 対象を捕獲する
		mHitEnemy->Capture();

		// 進行方向をその場で反転
		mMoveVector = Vector3D::zero;

		mAutoMoveComp->ReverseVelocity();

		mReelState = ReelState::ReelState_Enemy;

		return;
	}
	// 対象がブロックだった場合
	else if (oppAtt == ColliderAttribute::ColAtt_Block)
	{
		// 当たったブロックを記録
		mHitBlock = opponent->GetOwner();

		// その場で停止
		mAutoMoveComp->SetVelocity(Vector3D::zero);
		
		mMoveVector = Vector3D::zero;

		// プレイヤーを動かす方向を計算させる
		mNinjaArts->CalculateDashVector();

		// 方向変数の初期化
		mLaunchedXDirection = 0;

		mReelState = ReelState::ReelState_Block;

		return;
	}
	// 対象がプレイヤーの場合
	// （状況として、撃ち出した瞬間か、戻ってきたときか、ブロックのもとにプレイヤーを連れてきたときに限定される）
	else if (oppAtt == ColliderAttribute::ColAtt_Player)
	{
		// 戻ってる状態か、ブロックにヒットしている状態なら非アクティブに
		if (mAutoMoveComp->IsNowReverse() || mReelState == ReelState::ReelState_Block)
		{
			OnCompleteBringingUser();
		}
	}
}

void ReelStringEdgeActor::OnTouching(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
	// アトリビュート取得
	ColliderAttribute oppAtt = opponent->GetColliderAttribute();

	// 対象がプレイヤーの場合
	if (oppAtt == ColliderAttribute::ColAtt_Player)
	{
		// 戻ってる状態か、ブロックにヒットしている状態なら非アクティブに
		if (mAutoMoveComp->IsNowReverse() || mReelState == ReelState::ReelState_Block)
		{
			OnCompleteBringingUser();
		}
	}
}

void ReelStringEdgeActor::Launch(bool lookRight)
{
	// 各種変数初期化
	mPosition = mNinjaArts->GetUserPlayer()->GetPosition();

	mDistance = mMoveVector = Vector3D::zero;

	mHitBlock = mHitEnemy = nullptr;

	mReelState = ReelState::ReelState_Invalid;

	// 撃ち出し方向記録
	mLaunchedXDirection = (lookRight) ? 1 : -1;

	// 撃ち出し方向に応じてアクターを回転
	float angleDegrees = ((mLaunchedXDirection == 1) ? 0.0f : 180.0f);
	mRotationAngle = Common::DegToRad(angleDegrees);

	// 自動移動の方向を設定
	Vector3D v = mAutoMoveVector;
	v.x *= mLaunchedXDirection;

	mAutoMoveComp->SetVelocity(v);

	// コライダー有効化
	mCollider->SetActive(true);

	// アクティブ化
	SetActive(true);

	// 糸のアクティブ化
	mStringActor->SetActive(true);
}

void ReelStringEdgeActor::Cancel()
{
	// プレイヤーをブロックに動かした場合と同じ挙動を行う
	OnCompleteBringingUser();
}

void ReelStringEdgeActor::SetBeyondSceneFlag(bool value)
{
	Actor::SetBeyondSceneFlag(value);

	mStringActor->SetBeyondSceneFlag(value);
}

void ReelStringEdgeActor::OnCompleteBringingUser()
{
	// 非アクティブ化
	SetActive(false);

	// 忍術クラスに術が終了したことを教える。
	mNinjaArts->TellEndNinjaArts();

	// 敵が生きており、敵を捕らえていれば、捕えた状態から解放する
	if (mHitEnemy && mHitEnemy->GetLife() > 0)
	{
		mHitEnemy->LetGo();
	}
}
