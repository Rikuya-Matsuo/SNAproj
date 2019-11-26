#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute):
	ComponentBase(owner, 300),
	mAttribute(colAttribute)
{
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::Update()
{
}

void BoxColliderComponent::CalculateWorldBox()
{
	// �I�u�W�F�N�g��ԃ{�b�N�X�Ƀ��Z�b�g
	mWorldBox = mObjectBox;

	// �X�P�[�����v�Z�ɓ����
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	// ��]�i���ƂŎ������Ⴗ�j

	// ���s�ړ�
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}
