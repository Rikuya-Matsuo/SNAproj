﻿#pragma once
#include "StageBase.h"

class TestStage : public StageBase
{
public:
	TestStage();
	~TestStage();

	// ファイル名に何を入れてもテスト用のステージを生成する
	void Load(const std::string & fileName) override;

private:
	// クラスの定義内で使う定数である故、クラス内で初期化する
	static const Uint8 mTestBlockMassX = 32;
	static const Uint8 mTestBlockMassY = 4;

	Uint8 mTestBlocks[mTestBlockMassY][mTestBlockMassX];
};
