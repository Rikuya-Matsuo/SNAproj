#include "GoalBoxActor.h"
#include "System.h"
#include "Mesh.h"
#include "BoxColliderComponent.h"
#include "GameScene.h"

GoalBoxActor::GoalBoxActor():
	Actor()
{
	// �u���b�N�Ɏg���Ă�����̂Ɠ������b�V�����擾
	// �`��͍s��Ȃ��̂Ń��b�V���ɃA�N�^�[���͕K�v�Ȃ��B�����ɏ�������
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Cube.gpmesh", this);
	mesh->DeleteActorInfo(this);

	// ���b�V������R���W�����𐶐�
	AABB box = mesh->GetCollisionBox();
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	bcc->SetObjectBox(box);

	// �d�͖���
	SetAffectGravityFlag(false);

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
