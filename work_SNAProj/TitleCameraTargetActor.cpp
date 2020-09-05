#include "TitleCameraTargetActor.h"
#include "AutoMoveComponent.h"

TitleCameraTargetActor::TitleCameraTargetActor():
	Actor(),
	mAutoMoveComponent(new AutoMoveComponent(this))
{
}

TitleCameraTargetActor::~TitleCameraTargetActor()
{
}

void TitleCameraTargetActor::SetVelocity(const Vector3D & moveVec)
{
	mAutoMoveComponent->SetVelocity(moveVec);
}
