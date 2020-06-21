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
	virtual void ProcessInput(const uint8_t * keys);		//“ü—Í‚ğó‚¯æ‚éˆ—
	virtual void HandleKeyPress(int key);
};
