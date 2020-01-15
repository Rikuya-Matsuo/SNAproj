#pragma once
#include "Actor.h"

class MeshComponent;

class TitleBoard : public Actor
{
public:
	TitleBoard();
	~TitleBoard();

protected:
	MeshComponent * mMeshComp;
};
