#pragma once
#include <SDL/SDL.h>

class Shader;

class UI
{
public:
	UI();
	virtual ~UI();

	virtual void Update();
	virtual void Draw(Shader * shader);
	virtual void ProcessInput(const uint8_t * keys);		//入力を受け取る処理
	virtual void HandleKeyPress(int key);
};
