#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute):
	ColliderComponentBase(owner, colAttribute, ColliderShape::ColShape_Box)
{
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::Update()
{
	CalculateWorldBox();
}

void BoxColliderComponent::CalculateWorldBox()
{
	// オブジェクト空間ボックスにリセット
	mWorldBox = mObjectBox;

	// スケールを計算に入れる
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	// 回転
	if (mFlags_CCBase & mRotatableFlagMask)
	{
		mWorldBox.Rotate(mOwner->GetRotation());
	}

	// 平行移動
	Vector3D trans = mOwner->GetPosition();
	//trans += mOwner->GetMoveVector();
	mWorldBox.mMin += trans;
	mWorldBox.mMax += trans;
}
