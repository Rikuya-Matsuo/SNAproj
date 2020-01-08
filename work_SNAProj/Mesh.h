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

class Actor;
class Renderer;
class Texture;
class VertexArray;
class AnimationChips;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Load(const std::string& fileName, Renderer* renderer, const Actor * actor);   	// メッシュのロード・アンロード
	void Unload();

	void Update(const Actor * actor);

	bool LoadTexture(const std::string& fileName, Renderer* renderer, const Actor * actor);

	bool LoadDivTexture(const std::string& fileName, Renderer* renderer, const Actor * actor, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex);

	VertexArray* GetVertexArray() { return mVertexArray; }            // メッシュの頂点配列の取得
	Texture* GetTexture(const Actor * actor) const { return mCurrentTexture.at(actor); }
	Texture* GetAnimFrameTexture(const Actor* actor, int index);                              // アニメーションのコマを取得(いるかな、これ……)
	const std::string& GetShaderName() const { return mShaderName; }        // シェーダー名の取得

	float GetRadius() const { return mRadius; }                             // バウンディングスフィアの半径を取得

	const AABB & GetCollisionBox() const { return mBox; }

	AnimationChips * GetAnimChips(const Actor* actor, int index);

	void SetAnimChips(AnimationChips * anim, const Actor * actor, int index) { mAnimations[actor][index] = anim; }

	void SetAnimModeFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mAnimModeFlagMask); }
	bool GetAnimModeFlag() const { return mFlags & mAnimModeFlagMask; }

	void SetAnimIndex(const Actor * actor, int index) { mActiveAnimIndex[actor] = index; }

	bool GetAnimLoopEndFlag() const { return mFlags & mAnimLoopEndFlagMask; }

	bool GetIsBoardFlag() const { return mFlags & mIsBoardFlagMask; }

	void SetIsBoardFlagTrue() { mFlags |= mIsBoardFlagMask; }

	void DuplicateDefaultTexture(const Actor * actor) { mCurrentTexture[actor] = mDefaultTexture[actor] = mDefaultTexture.begin()->second; }

private:
	static const char* mBoardMeshPath;

	typedef Uint8 FlagType;
	static const FlagType mAnimModeFlagMask;
	static const FlagType mAnimLoopEndFlagMask;
	static const FlagType mIsBoardFlagMask;


	FlagType mFlags;

	// メッシュのテクスチャ
	std::unordered_map<const Actor *, Texture *> mDefaultTexture;

	std::unordered_map<const Actor *, Texture *> mCurrentTexture;

	typedef std::unordered_map<int, AnimationChips *> AnimChipCollection;

	std::unordered_map<const Actor*, AnimChipCollection> mAnimations;

	std::unordered_map<const Actor *, int> mActiveAnimIndex;

	// アニメーションが持ってるテクスチャをインデックスで管理
	//std::unordered_map<int, std::vector<Texture *>> mAnimTex;

	VertexArray* mVertexArray;                                        // メッシュの頂点配列

	std::string mShaderName;	                                            // メッシュ指定のシェーダー
	float mRadius;	                                                        // バウンディングスフィアの半径
	float mSpecPower;	                                                    // 表面のスペキュラー値

	AABB mBox;																// バウンディングボックス

};