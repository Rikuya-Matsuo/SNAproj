#include "FireActor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

const float FireActor::mTimeLimit = 3.0f;

FireActor::FireActor(Player * user):
	mTimer(0.0f)
{
	// ���b�V���ǂݍ���
	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mesh->LoadDivTexture("Assets/flame_parts1.png", System::GetInstance().GetRenderer(), this,
		9, 3, 3, 1024, 1024, 0.05f, 0);
	
	// ���b�V���R���|�[�l���g���[�h
	MeshComponent * mc = new MeshComponent(this, 100, false);
	mc->SetMesh(mesh);

	mScale = 100.0f;

	// ��A�N�e�B�u
	SetActive(false);

	// �f�o�b�O�̂��߁A�d�͖���
	SetAffectGravityFlag(false);
}

FireActor::~FireActor()
{
}

void FireActor::UpdateActor0()
{
	// �^�C�}�[�v��
	mTimer += System::GetInstance().GetDeltaTime();

	// �K�莞�Ԃ��߂���Δ�A�N�e�B�u��
	if (mTimer >= mTimeLimit)
	{
		SetActive(false);
	}
}

void FireActor::OnBecomeActive()
{
	// ���N���X�̓��֐����Ă�
	Actor::OnBecomeActive();

	// �^�C�}�[������
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
