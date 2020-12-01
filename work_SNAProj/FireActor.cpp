#include "FireActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

const float FireActor::mTimeLimit = 3.0f;

FireActor::FireActor(Player * user):
	mTimer(0.0f)
{
	// メッシュ読み込み
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadDivTexture("Assets/flame_parts1.png", System::GetInstance().GetRenderer(), this,
		9, 3, 3, 1024, 1024, 0.05f, 0);
	
	// メッシュコンポーネントロード
	MeshComponent * mc = new MeshComponent(this, 100, false);
	mc->SetMesh(mesh);

	mScale = 100.0f;

	// 非アクティブ
	SetActive(false);

	// デバッグのため、重力無効
	SetAffectGravityFlag(false);
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

void FireActor::OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
}

void FireActor::OnTouching(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
}

void FireActor::OnApart(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
}
