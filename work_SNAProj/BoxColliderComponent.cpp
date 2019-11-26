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
	// オブジェクト空間ボックスにリセット
	mWorldBox = mObjectBox;

	// スケールを計算に入れる
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	// 回転（あとで実装しゃす）

	// 平行移動
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}
