#pragma once
#include "Actor.h"

class AutoMoveComponent;

// タイトル画面でステージを映すため、透明で自動で動くアクター
class TitleCameraTargetActor : public Actor
{
public:
	TitleCameraTargetActor();
	~TitleCameraTargetActor();

	void SetVelocity(const Vector3D & moveVec);

private:
	AutoMoveComponent * mAutoMoveComponent;
};
