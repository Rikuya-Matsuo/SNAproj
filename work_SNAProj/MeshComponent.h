// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ComponentBase.h"
#include <cstddef>

class MeshComponent : public ComponentBase
{
public:
	MeshComponent(class Actor* owner, bool isSkeletal = false);       // メッシュコンポーネントコンストラクタ。（要：親アクター）
	~MeshComponent();

	void Update() override;

	virtual void Draw(class Shader* shader);                          // メッシュコンポーネントの描画
	virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }          // メッシュコンポーネントで使用するメッシュのセット
	void SetTextureIndex(size_t index) { mTextureIndex = index; }     // テクスチャのセット

	void SetVisible(bool visible) { mVisible = visible; }             // 表示フラグのセット
	bool GetVisible() const { return mVisible; }                      // 表示フラグのゲット

	bool GetIsSkeletal() const { return mIsSkeletal; }                // スケルタルモデルかどうかの取得
protected:
	class Mesh* mMesh;                                                // メッシュオブジェクトの取得
	size_t mTextureIndex;                                             // テクスチャーのインデックス
	bool mVisible;                                                    // 表示フラグ
	bool mIsSkeletal;                                                 // スケルタルモデルかどうかのフラグ
};