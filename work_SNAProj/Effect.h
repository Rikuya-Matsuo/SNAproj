#pragma once
#include "Actor.h"

class Mesh;
class MeshComponent;

class Effect : public Actor
{
public:
	Effect(const char * texPath, int priority = 0);
	~Effect();

	void SetAppearSecond(float second) { mAppearSecond = second; }

protected:
	static const float mDepth;

	Mesh * mMesh;
	MeshComponent * mMeshComp;

	// 出現時間
	float mAppearSecond;

	float mTimer;

	void UpdateActor0() override;

	void OnBecomeActive() override;

	void OnBecomeNotActive() override;

	virtual void StopProcess();
};

