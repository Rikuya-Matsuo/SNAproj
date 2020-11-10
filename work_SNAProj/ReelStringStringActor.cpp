#include "ReelStringStringActor.h"
#include "NAReelString.h"
#include "ReelStringEdgeActor.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "System.h"
#include "Player.h"
#include <cmath>

ReelStringStringActor::ReelStringStringActor(NAReelString * ninjaArts, ReelStringEdgeActor * edge):
	mNinjaArts(ninjaArts),
	mEdge(edge)
{
	// 更新順はEdgeよりも後
	SetPriority(mEdge->GetPriority() + 50);

	// メッシュのロード
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	msh->LoadTexture("Assets/ReelString.png", System::GetInstance().GetRenderer(), this);

	MeshComponent* meshCmp = new MeshComponent(this, 100, false);
	meshCmp->SetMesh(msh);

	// スケール値固定フラグの設定（縦の大きさだけ固定）
	SetFixScaleFlag(false, false, true);

	// z軸のスケール値を固定
	SetFixedScale(Vector3D(0.0f, 0.0f, 10.0f));

	// 重力不適用
	SetAffectGravityFlag(false);

	// 非アクティブ化
	SetActive(false);
}

void ReelStringStringActor::UpdateActor0()
{
	// Edgeが非アクティブであれば非アクティブ化
	if (!mEdge->GetActiveFlag())
	{
		SetActive(false);
	}

	// プレイヤー取得
	Player * player = mNinjaArts->GetUserPlayer();

	// プレイヤーから見たEdgeのx座標の差
	float distance = mEdge->GetPosition().x - player->GetPosition().x;

	// 位置座標をプレイヤーとEdgeの間に設定
	SetPosition(player->GetPosition());
	mPosition.x += distance / 2.0f;

	// 長さの分大きさを拡大する
	SetScale(fabsf(distance));
}
