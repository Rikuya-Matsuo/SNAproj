#pragma once
#include "SDL/SDL_image.h"
#include "GL/glew.h"
#include "Vector.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	void Draw(const Vector2D& pos);

	void ConvertSDLSurface(SDL_Surface * surface);

private:
	GLuint * mPixels;

	GLuint mWidth;
	GLuint mHeight;
};
