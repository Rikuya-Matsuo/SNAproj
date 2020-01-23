#pragma once
#include "SDL/SDL.h"
#include <string>

class StageBase
{
public:
	StageBase();
	~StageBase();

	virtual void Load(const std::string & fileName);

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

	std::string mBlockTexturePath;

	std::string mFloorTexturePath;

	float mFloorScale;

	float mFloorHeight;

	// ブロック配置配列に基づいてブロックのインスタンスを生成する。
	void Construct();
};
