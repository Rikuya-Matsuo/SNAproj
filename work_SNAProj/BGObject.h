#pragma once
#include "Actor.h"

class MeshComponent;

class BGObject : public Actor
{
public:
	BGObject(const std::string & meshFileName);
	~BGObject();

	const Vector3D & GetModelSize() const { return mModelSize; }

	bool GetSucessToLoadFlag() const { return mSucessToLoadFlag; }

private:
	bool mSucessToLoadFlag;

	Vector3D mModelSize;
};
