#pragma once
#include "DrawComponentBase.h"
#include <string>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

class Sprite;

class SpriteComponent : public DrawComponentBase
{
public:
	SpriteComponent(Actor * owner);
	virtual ~SpriteComponent();

	void Update() override;
	void Draw() const override;

	void LoadSprite(const std::string & filePath);

protected:
	int mTextureWidth;
	int mTextureHeight;

	//Sprite * mSprite;

private:
	SDL_Texture * mTexture;
};
