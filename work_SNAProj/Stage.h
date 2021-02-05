﻿#pragma once
#include "SDL/SDL.h"
#include <string>
#include <list>
#include <unordered_map>
#include <fstream>

class Actor;
class Block;

class Stage
{
public:
	// ブロックの種類のIDの型
	typedef Uint16 BlockKindIDType;

	Stage();
	~Stage();

	static const BlockKindIDType mNormalBlockID;

	static const BlockKindIDType mGoalID;

	static const BlockKindIDType mEmptyID;

	static const BlockKindIDType mBombID;

	static const BlockKindIDType mFriableID;

	virtual void LoadMap(const std::string & mapFilePath, const std::string & blockTextureFilePath, const std::string & floorTextureFilePath);

	// 背景物の位置データcsvから、背景物を読み込む
	// generatedActors :	生成されたオブジェクトへのポインタの配列へのポインタへのポインタ。
	//						要するにActor**型変数Xへのポインタを入力することで、XはActor*型の配列となる。
	//						この機能を使った場合は後でdelete[] X;　と、メモリを解放すること
	virtual int LoadBGObjectMap(const std::string & bgObjMapFilePath, float xStartPos, float groundHeight, float depth, float xEmptyCellScale, float yEmptyCellScale, Actor *** generatedActors = nullptr);

	static void SetBlockScale(float scale) { mBlockScale = scale; }

	static float GetBlockScale() { return mBlockScale; }

	float GetFloorHeight() const { return mFloorHeight; }

	float GetGoalLine() const { return mGoalLineX; }

	int GetBlockMassX() const { return mBlockMassX; }
	int GetBlockMassY() const { return mBlockMassY; }

	BlockKindIDType ** const GetBlocks() const { return mBlocks; }

	bool GetGoalBlockFlag() const { return mGoalBlockFlag; }

	void ChangeToEmptyBlock(Uint16 x, Uint16 y);

private:
	BlockKindIDType ** mBlocks;

	int mBlockMassX;
	int mBlockMassY;

	static float mBlockScale;

	float mGoalLineX;

	float mFloorHeight;

	// マップ内にクリアブロックが存在するかのフラグ
	// Construct関数内で設定される
	bool mGoalBlockFlag;

	// ステージデータ内の文字列をUint8に変換するためのデータ
	static std::unordered_map<std::string, BlockKindIDType> mBlockID;

	// マップデータのギミックの最大個数
	static const Uint8 mGimmickIDNum = 10;

	// ろうそく＆ギミックの番号ごとのID
	BlockKindIDType mCandleIDs[mGimmickIDNum];
	BlockKindIDType mVanishBlockIDs[mGimmickIDNum];

	// ブロック配置配列に基づいてブロックのインスタンスを生成する。
	void Construct(const std::string & blockTextureFilePath, const std::string & floorTextureFilePath);

	struct BGObjectPallet
	{
		std::string mModelFilePath;
		float mScale;
	};

	bool LoadBGObjectMapPallet(std::ifstream & file, std::unordered_map<std::string, BGObjectPallet> & ret);

	int LoadBGObjectMapPosition(std::ifstream & file, const std::unordered_map<std::string, BGObjectPallet> & pallet,
		float xScale, float yScale, float zPos, float xEmptyCellScale, float yEmptyCellScale, Actor *** generatedActors);

	// 数値に応じた種類のブロックの生成を行う一種のファクトリメソッド
	// Construct関数内で使う
	Block * GenerateBlock(BlockKindIDType num, const std::string & blockTexFilePath, bool isGroundBlock);

	static BlockKindIDType GetBlockKindID(const std::string & str);
};
