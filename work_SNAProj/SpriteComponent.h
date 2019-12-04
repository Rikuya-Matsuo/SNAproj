#pragma once
#include "DrawComponentBase.h"
#include <string>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

class SpriteComponent : public DrawComponentBase
{
public:
	SpriteComponent(Actor * owner);
	virtual ~SpriteComponent();

	void Update() override;
	void Draw() const override;

	void LoadTexture(const std::string & filePath);

protected:
	int mTextureWidth;
	int mTextureHeight;

private:
	GLuint mTexture;
};
