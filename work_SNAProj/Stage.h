#pragma once
#include "SDL/SDL.h"
#include <string>
#include <list>

class BGObject;

class Stage
{
public:
	Stage();
	~Stage();

	virtual void LoadMap(const std::string & mapFilePath, const std::string & blockTextureFilePath, const std::string & floorTextureFilePath);

	// 背景物の位置データcsvから、背景物を読み込む
	// xScale	: csvの１セルでどのくらい水平座標が違うかの、目盛りの大きさ
	// yScale	: csvの１セルでどのくらい鉛直座標が違うかの、目盛りの大きさ
	virtual void LoadBGObjectMap(const std::string & bgObjMapFilePath, float xScale, float yScale);

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
};
