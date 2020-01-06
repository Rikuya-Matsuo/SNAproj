#pragma once
#include "SDL/SDL.h"
#include <string>

class StageBase
{
public:
	StageBase();
	~StageBase();

	// 未実装
	virtual void Load(const std::string & fileName);

	static void SetBlockScale(float scale) { mBlockScale = scale; }

protected:
	Uint8 ** mBlocks;

	int mBlockMassX;
	int mBlockMassY;

	static float mBlockScale;

	std::string mBlockTexturePath;

	std::string mFloorTexturePath;

	float mFloorScale;

	// ブロック配置配列に基づいてブロックのインスタンスを生成する。
	void Construct();
};
