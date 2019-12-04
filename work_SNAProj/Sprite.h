#pragma once
#include "SDL/SDL_image.h"
#include "GL/glew.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	void Draw();

	void ConvertSDLSurface(SDL_Surface * surface);

private:
	GLuint * mPixels;

	GLuint mWidth;
	GLuint mHeight;
};
