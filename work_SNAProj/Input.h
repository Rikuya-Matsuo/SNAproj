﻿#pragma once
#include "SDL/SDL.h"

class Input final
{
public:
	~Input();
	
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	void Update();

	void LastUpdate() { SDL_memcpy(mPrevStates, mStates, SDL_NUM_SCANCODES); }

	bool GetKey(int scanCode) const { return mStates[scanCode]; }

	bool GetQuitEventFlag() const { return mQuitEventFlag; }

	bool GetKeyPressDown(int scanCode) const;

	bool GetKeyPressUp(int scanCode) const;

	bool GetGamePadButton(SDL_GameControllerButton button) const { return mGamePadButtonFlags & (0x0001 << static_cast<int>(button)); }

private:
	Input();

	bool mQuitEventFlag;

	const Uint8 * mStates;

	Uint8 mPrevStates[SDL_NUM_SCANCODES];

	SDL_GameController * mGamePad;

	char * mGamePadMapping;

	Uint16 mGamePadButtonFlags;

	void UpdateGamePad();
};
