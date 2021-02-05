#include "GoalBoxActor.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "GameScene.h"

GoalBoxActor::GoalBoxActor():
	Block("", false)
{
	mFlags |= mStopDrawFlagMask_Base;

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
