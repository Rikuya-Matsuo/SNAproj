#pragma once
#include "SceneBase.h"

class UIScreen;

class TestScene01 : public SceneBase
{
public:
	TestScene01();
	~TestScene01();

	void Update() override;

private:
	UIScreen * mTestUI;
};

