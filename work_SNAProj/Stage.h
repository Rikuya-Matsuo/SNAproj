#pragma once
#include "SDL/SDL.h"
#include <string>

class Stage
{
public:
	Stage();
	~Stage();

	virtual void Load(const std::string & mapFilePath, const std::string & blockTextureFilePath, const std::string & floorTextureFilePath);

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
