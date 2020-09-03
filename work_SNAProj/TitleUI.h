#pragma once
#include "UIScreen.h"

class Texture;

class TitleUI : public UIScreen
{
public:
	TitleUI();
	~TitleUI();

	void Draw(Shader * shader) const override;

private:
	Texture * mLogo;
};

