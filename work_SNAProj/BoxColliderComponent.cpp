#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute):
	ComponentBase(owner, 300),
	mAttribute(colAttribute)
{
}

BoxColliderComponent::~BoxColliderComponent()
{
}

