#pragma once
#include "Actor.h"

class MeshComponent;
class BoxColliderComponent;
class Stage;

class Block : public Actor
{
public:
	Block(const std::string & texturePath, bool isGroundBlock);
	~Block();

	static const float mModelSize;

	void UpdateActor0() override;
	
	void UpdateActor1() override;

	void SetPositionOnStage(Uint16 x, Uint16 y);

	void SetStage(Stage * st) { mBelongStage = st; }

protected:
	MeshComponent * mMeshComponent;

	BoxColliderComponent * mCollider;

	bool mPrevInCameraFlag;

	Stage * mBelongStage;

	struct PositionOnStage
	{
		Uint16 mX;
		Uint16 mY;
	} mPositionOnStage;

	void VanishFromStage() const;
};

