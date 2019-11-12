#include "Input.h"

Input::Input() :
	mQuitEventFlag(false)
{
	mStates = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		mPrevStates[i] = 0;
	}
}

Input::~Input()
{
}

void Input::Update()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_QUIT)
		{
			mQuitEventFlag = true;
		}
	}
}

bool Input::GetKeyPressDown(int scanCode) const
{
	bool ret = (GetState(scanCode) && !mPrevStates[scanCode]);
	return ret;
}

bool Input::GetKeyPressUp(int scanCode) const
{
	bool ret = (!GetState(scanCode) && mPrevStates[scanCode]);
	return ret;
}
