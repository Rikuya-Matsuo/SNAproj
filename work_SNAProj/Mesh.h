// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Collision.h"
#include <vector>
#include <string>
#include <unordered_map>

class Renderer;
class Texture;
class VertexArray;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Load(const std::string& fileName, Renderer* renderer);   	// メッシュのロード・アンロード
	void Unload();                                                         	

	int LoadTexture(const std::string& fileName, Renderer* renderer);

	VertexArray* GetVertexArray() { return mVertexArray; }            // メッシュの頂点配列の取得
	Texture* GetTexture(size_t index);                                // 指定されたインデックスからテクスチャの取得
	const std::string& GetShaderName() const { return mShaderName; }        // シェーダー名の取得

	float GetRadius() const { return mRadius; }                             // バウンディングスフィアの半径を取得

	const AABB & GetCollisionBox() const { return mBox; }

	bool IsAssigned(int index);

private:
	// メッシュのテクスチャ
	std::unordered_map<int, Texture *> mTextures;

	VertexArray* mVertexArray;                                        // メッシュの頂点配列

	std::string mShaderName;	                                            // メッシュ指定のシェーダー
	float mRadius;	                                                        // バウンディングスフィアの半径
	float mSpecPower;	                                                    // 表面のスペキュラー値

	AABB mBox;																// バウンディングボックス
};