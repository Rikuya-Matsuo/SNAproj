#pragma once
#include "UIScreen.h"

class AnimationChips;

class LoadingTimeUI : public UIScreen
{
public:
	LoadingTimeUI();
	~LoadingTimeUI();

	void Update() override;

	void Draw(Shader * shader) const override;

private:
	AnimationChips * mAnimation;

	Texture* mTextTexture;
};
