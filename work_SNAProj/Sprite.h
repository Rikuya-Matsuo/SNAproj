#pragma once
#include "SDL/SDL_image.h"
#include "GL/glew.h"
#include "Vector.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	void Draw(const Vector2D& pos) const;

	void ConvertSDLSurface(SDL_Surface * surface);

	void XFlip();

private:
	typedef Uint8 FlagType;
	static const FlagType mXFlipFlagMask;
	//static const FlagType mPrevXFlipFlagMask;

	GLuint * mPixels;

	GLuint mWidth;
	GLuint mHeight;

	Uint8 mFlags;
};
