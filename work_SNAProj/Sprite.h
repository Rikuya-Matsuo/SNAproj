#pragma once
#include "SDL/SDL.h"
#include "Vector.h"
#include "BitFlagFunc.h"

class Texture;

class Sprite
{
public:
	Sprite(const char * texPath = nullptr);
	~Sprite();

	void Draw();

	void SetPosition(const Vector2D & pos) { mPosition = pos; }
	const Vector2D& GetPosition() const { return mPosition; }

	void SetSize(const Vector2D & size) { mSize = size; }
	const Vector2D& GetSize() const { return mSize; }

	void SetDrawFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mDrawFlagMask); }

protected:
	typedef Uint8 FlagType;
	static const FlagType mDrawFlagMask;


	FlagType mFlags;

	Texture * mTexture;

	Vector2D mPosition;

	Vector2D mSize;

	void CalculatePosition(const Vector2D& pos, Vector2D & result);
};

