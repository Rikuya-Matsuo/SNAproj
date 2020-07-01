#pragma once
#include <SDL/SDL.h>
#include <string>
#include <functional>
#include "Vector.h"

class Shader;
class Texture;
class Font;

class UIScreen
{
public:
	UIScreen();
	virtual ~UIScreen();

	virtual void Update();
	virtual void Draw(Shader * shader);
	virtual void ProcessInput(const uint8_t * keys);		//入力を受け取る処理
	virtual void HandleKeyPress(int key);

	void Close();

	void SetTitle(const std::string & text,
		const Vector3D & color,
		int pointSize = 40);

	void AddButton(const std::string & name,
		std::function<void()> onClick);

protected:
	typedef uint8_t FlagType;
	FlagType mFlags_base;

	Font * mFont;
	Texture * mTitle;
	Texture * mBackGround;
	Texture * mButtonOn;
	Texture * mButtonOff;

	Vector2D mTitlePos;
	Vector2D mNextButtonPos;
	Vector2D mBGPos;

	void DrawTexture(Shader * shader, Texture * texture,
		const Vector2D & offset = Vector2D::zero,
		float scale = 1.0f);

	void SetRelativeMouseMode(bool relativeFlag);
};
