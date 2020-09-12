﻿#pragma once
#include "SDL/SDL.h"
#include <string>
#include <list>
#include <unordered_map>
#include <fstream>

class Actor;

class Stage
{
public:
	Stage();
	~Stage();

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

	Uint8 **const GetBlocks() const { return mBlocks; }

protected:
	Uint8 ** mBlocks;

	int mBlockMassX;
	int mBlockMassY;

	static float mBlockScale;

	float mGoalLineX;

	float mFloorHeight;

	// ブロック配置配列に基づいてブロックのインスタンスを生成する。
	void Construct(const std::string & blockTextureFilePath, const std::string & floorTextureFilePath);

private:
	struct BGObjectPallet
	{
		std::string mModelFilePath;
		float mScale;
	};

	bool LoadBGObjectMapPallet(std::ifstream & file, std::unordered_map<std::string, BGObjectPallet> & ret);

	int LoadBGObjectMapPosition(std::ifstream & file, const std::unordered_map<std::string, BGObjectPallet> & pallet, float xScale, float yScale, float zPos, float xEmptyCellScale, float yEmptyCellScale, Actor *** generatedActors);

	void ClearPallet(BGObjectPallet & pallet);
};
