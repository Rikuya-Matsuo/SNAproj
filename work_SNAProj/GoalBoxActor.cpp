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

	// �ڐG���肩��̉����Ԃ����󂯂Ȃ�
	mFlags &= ~mMovalFlagMask_Base;
}

GoalBoxActor::~GoalBoxActor()
{
}

void GoalBoxActor::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	// �Q�[���V�[�������łȂ���Ί֐��𔲂���
	if (!mBelongScene->IsGameScene())
	{
		return;
	}

	bool isOpponentPlayer = (opponent->GetColliderAttribute() == ColliderAttribute::ColAtt_Player);

	if (isOpponentPlayer)
	{
		// �Q�[���V�[���ł��邱�Ƃ͊֐��̏��߂Ɋm�F�ς�
		GameScene * gScene = dynamic_cast<GameScene*>(mBelongScene);

		// �Q�[���̃N���A���V�[���ɒʒB����
		gScene->TellStageClear();
	}
}
