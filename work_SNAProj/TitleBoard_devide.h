#pragma once
#include "Actor.h"

class Mesh;
class MeshComponent;
class CompletionMeshActor;

class TitleBoard : public Actor
{
public:
	TitleBoard();
	~TitleBoard();

protected:
	static const size_t mMeshActorMass = 4;

	CompletionMeshActor * mMeshActor[mMeshActorMass];

	Mesh * mMesh;

	void UpdateActor0() override;
};
