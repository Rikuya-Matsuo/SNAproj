#pragma once
#include "UIScreen.h"

class TestUIScreen : public UIScreen
{
public:
	TestUIScreen();
	~TestUIScreen();

	void Draw(Shader * shader) const override;

private:
	Texture * mTexture;
};

