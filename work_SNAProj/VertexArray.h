// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
class VertexArray
{
public:
	// 頂点レイアウト列挙
    enum Layout
	{
		PosNormTex,     // 位置&法線&テクスチャUV を持ったフォーマット
		PosNormSkinTex  // 位置&法線& "スキン用の影響ボーン＆重み情報" & テクスチャUV 
	};

	VertexArray(const void* verts, unsigned int numVerts, Layout layout,  // 頂点配列コンストラクタ 
		const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	void SetActive();                                                     // この頂点配列をアクティブにして描画で使用する
	unsigned int GetNumIndices() const { return mNumIndices; }            // インデックス数を取得する
	unsigned int GetNumVerts() const { return mNumVerts; }                // 頂点数を取得する
private:

	unsigned int mNumVerts;                      // 頂点数
	unsigned int mNumIndices;                    // インデックス数 （ポリゴン面数×３)
	unsigned int mVertexBuffer;                  // 頂点バッファID（OpenGLに登録時に付与される）
	unsigned int mIndexBuffer;                   // インデックスバッファID (OpenGL登録時に付与）
	unsigned int mVertexArray;                   // 頂点配列オブジェクトID
}; 