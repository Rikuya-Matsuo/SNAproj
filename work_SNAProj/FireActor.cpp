#include "FireActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BoxColliderComponent.h"
#include "Player.h"
#include "EnemyBase.h"

const float FireActor::mTimeLimit = 3.0f;

FireActor::FireActor(Player * user):
	mTimer(0.0f),
	mUser(user)
{
	// メッシュ読み込み
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadDivTexture("Assets/flame_parts1.png", System::GetInstance().GetRenderer(), this,
		9, 3, 3, 1024, 1024, 0.05f, 0);
	
	// メッシュコンポーネント生成
	MeshComponent * mc = new MeshComponent(this, 100, false);
	mc->SetMesh(mesh);

	// ボックスコライダー生成
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Fire);
	AABB colBox = mesh->GetCollisionBox();
	bcc->SetObjectBox(colBox);

	// 落下スピード設定
	mFallSpeedRate = 15.0f;
	mFallSpeedMax = 3.0f;

	// 非アクティブ
	SetActive(false);

	// 更新順をプレイヤーよりも後に設定
	SetPriority(mUser->GetPriority() + 50);
}

FireActor::~FireActor()
{
}

void FireActor::UpdateActor0()
{
	// タイマー計測
	mTimer += System::GetInstance().GetDeltaTime();

	// 規定時間が過ぎれば非アクティブ化
	if (mTimer >= mTimeLimit)
	{
		SetActive(false);
	}
}

void FireActor::OnBecomeActive()
{
	// 基底クラスの同関数を呼ぶ
	Actor::OnBecomeActive();

	// タイマー初期化
	mTimer = 0.0f;
}

void FireActor::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();

	if (opponentAtt == ColliderAttribute::ColAtt_Enemy)
	{
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());

		enemy->Damage(1);
	}
}

void FireActor::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}

void FireActor::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}
