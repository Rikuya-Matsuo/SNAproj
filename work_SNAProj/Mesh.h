// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include "Collision.h"
#include "BitFlagFunc.h"
#include <vector>
#include <string>
#include <unordered_map>

class Renderer;
class Texture;
class VertexArray;
class AnimationChips;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Load(const std::string& fileName, Renderer* renderer);   	// メッシュのロード・アンロード
	void Unload();

	void Update();

	bool LoadTexture(const std::string& fileName, Renderer* renderer);

	bool LoadDivTexture(const std::string& fileName, Renderer* renderer, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex);

	VertexArray* GetVertexArray() { return mVertexArray; }            // メッシュの頂点配列の取得
	Texture* GetTexture() const { return mCurrentTexture; }
	Texture* GetAnimFrameTexture(int index);                              // アニメーションのコマを取得(いるかな、これ……)
	const std::string& GetShaderName() const { return mShaderName; }        // シェーダー名の取得

	float GetRadius() const { return mRadius; }                             // バウンディングスフィアの半径を取得

	const AABB & GetCollisionBox() const { return mBox; }

	AnimationChips * GetAnimChips(int index);

	void SetAnimModeFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mAnimModeFlagMask); }
	bool GetAnimModeFlag() const { return mFlags & mAnimModeFlagMask; }

	bool GetAnimLoopEndFlag() const { return mFlags & mAnimLoopEndFlagMask; }

private:
	typedef Uint8 FlagType;
	static const FlagType mAnimModeFlagMask;
	static const FlagType mAnimLoopEndFlagMask;


	FlagType mFlags;

	// メッシュのテクスチャ
	Texture * mDefaultTexture;

	Texture * mCurrentTexture;

	std::unordered_map<int, AnimationChips *> mAnimations;

	// アニメーションが持ってるテクスチャをインデックスで管理
	std::unordered_map<int, std::vector<Texture *>> mAnimTex;

	VertexArray* mVertexArray;                                        // メッシュの頂点配列

	std::string mShaderName;	                                            // メッシュ指定のシェーダー
	float mRadius;	                                                        // バウンディングスフィアの半径
	float mSpecPower;	                                                    // 表面のスペキュラー値

	AABB mBox;																// バウンディングボックス

	int mActiveAnimIndex;
};