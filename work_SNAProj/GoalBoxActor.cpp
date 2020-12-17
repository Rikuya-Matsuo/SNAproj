#include "GoalBoxActor.h"
#include "System.h"
#include "Mesh.h"
#include "BoxColliderComponent.h"
#include "GameScene.h"

GoalBoxActor::GoalBoxActor():
	Actor()
{
	// ブロックに使われているものと同じメッシュを取得
	// 描画は行わないのでメッシュにアクター情報は必要ない。即座に消去する
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Cube.gpmesh", this);
	mesh->DeleteActorInfo(this);

	// メッシュからコリジョンを生成
	AABB box = mesh->GetCollisionBox();
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	bcc->SetObjectBox(box);

	// 重力無効
	SetAffectGravityFlag(false);

	// 接触相手からの押し返しを受けない
	mFlags &= ~mMovalFlagMask_Base;
}

GoalBoxActor::~GoalBoxActor()
{
}

void GoalBoxActor::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// ゲームシーン所属でなければ関数を抜ける
	if (!mBelongScene->IsGameScene())
	{
		return;
	}

	bool isOpponentPlayer = (opponent->GetColliderAttribute() == ColliderAttribute::ColAtt_Player);

	if (isOpponentPlayer)
	{
		// ゲームシーンであることは関数の初めに確認済み
		GameScene * gScene = dynamic_cast<GameScene*>(mBelongScene);

		// ゲームのクリアをシーンに通達する
		gScene->TellStageClear();
	}
}
