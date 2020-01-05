// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ComponentBase.h"
#include "BitFlagFunc.h"
#include <cstddef>

class Shader;
class Mesh;

class MeshComponent : public ComponentBase
{
public:
	MeshComponent(class Actor* owner, int drawOrder, bool isSkeletal = false);       // メッシュコンポーネントコンストラクタ。（要：親アクター）
	~MeshComponent();

	void Update() override;

	virtual void Draw(Shader* shader);                          // メッシュコンポーネントの描画
	virtual void SetMesh(Mesh* mesh);          // メッシュコンポーネントで使用するメッシュのセット
	void SetTextureIndex(size_t index) { mTextureIndex = index; }     // テクスチャのセット

	void SetVisible(bool visible) { BitFlagFunc::SetFlagByBool(visible, mMeshCompFlags, mVisibleFlagMask); }             // 表示フラグのセット
	bool GetVisible() const { return mMeshCompFlags & mVisibleFlagMask; }                      // 表示フラグのゲット

	bool GetIsSkeletal() const { return mMeshCompFlags & mIsSkeletalFlagMask; }                // スケルタルモデルかどうかの取得

	int GetDrawOrder() const { return mDrawOrder; }

protected:
	typedef Uint8 FlagType;
	static const FlagType mVisibleFlagMask;
	static const FlagType mIsSkeletalFlagMask;
	static const FlagType mAnimationModeFlagMask;


	FlagType mMeshCompFlags;

	Mesh* mMesh;                                                // メッシュオブジェクトの取得

	size_t mTextureIndex;                                             // テクスチャーのインデックス

	const int mDrawOrder;
};