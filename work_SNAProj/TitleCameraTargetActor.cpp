#include "TitleCameraTargetActor.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"

TitleCameraTargetActor::TitleCameraTargetActor():
	Actor(),
	mAutoMoveComponent(new AutoMoveComponent(this))
{
	SetAffectGravityFlag(false);

	ClampSpeedComponent * clampComp = new ClampSpeedComponent(this);
	clampComp->SetClampDirectionFlags(true, true, true);
	float speedLimit = 30.0f;
	clampComp->SetLimit(Vector3D(speedLimit, speedLimit, speedLimit));
}

TitleCameraTargetActor::~TitleCameraTargetActor()
{
}

void TitleCameraTargetActor::SetVelocity(const Vector3D & moveVec)
{
	mAutoMoveComponent->SetVelocity(moveVec);
}
