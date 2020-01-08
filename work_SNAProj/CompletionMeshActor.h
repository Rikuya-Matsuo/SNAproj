#pragma once
#include "Actor.h"

class MeshComponent;
class Mesh;
class Renderer;

class CompletionMeshActor : public Actor
{
public:
	CompletionMeshActor(const Actor * owner, int drawOrder);
	~CompletionMeshActor();

	void LoadAnimation(const std::string & filePath, Renderer * renderer, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex);

	void SetAnimationIndex(int index) { mCurrentIndex = index; };

	Mesh * GetMesh() const { return mMesh; }

	const Vector3D & GetPositionOffset() const { return mPositionOffset; }
	void SetPositionOffset(const Vector3D & offset);

	void AdaptPosition() { mPosition = mOwner->GetPosition() + mPositionOffset; }

private:
	const Actor * mOwner;

	MeshComponent * mMeshComponent;

	Mesh * mMesh;

	Vector3D mPositionOffset;

	int mCurrentIndex;

	std::list<int> mAnimIndexList;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void UpdateTransformData();
};
