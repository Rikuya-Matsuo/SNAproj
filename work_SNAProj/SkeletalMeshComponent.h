/*
#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent : public MeshComponent
{
public:
	SkeletalMeshComponent(class Actor* owner);
	// Draw this mesh component                                              メッシュコンポーネントの描画
	void Draw(class Shader* shader) override;

	void Update(float deltaTime) override;

	// Setters                                                               スケルトンのセット
	void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

	// Play an animation. Returns the length of the animation                 アニメーションの再生。アニメーションの残り長さを返す
	float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
	void ComputeMatrixPalette();                                           // 行列パレットの計算

	MatrixPalette mPalette;                                                // 行列パレット
	const class Skeleton* mSkeleton;                                       // スケルトン
	const class Animation* mAnimation;                                     // アニメーション
	float mAnimPlayRate;                                                   // アニメーションの再生速度
	float mAnimTime;                                                       // アニメーション時間
};
*/