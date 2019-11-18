#include "Input.h"

Input::Input() :
	mQuitEventFlag(false),
	mGamePadButtonFlags(0)
{
	mStates = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		mPrevStates[i] = 0;
	}

	// ゲームコントローラー取得
	mGamePad = SDL_GameControllerOpen(0);
	if (mGamePad == NULL)
	{
		SDL_Log("Failed to get game controller.\n--%s--\n", SDL_GetError());
	}

	mGamePadMapping = SDL_GameControllerMapping(mGamePad);
	if (mGamePadMapping == NULL)
	{
		SDL_Log("Failed to get mapping of game controller.\n--%s--\n", SDL_GetError());
	}
}

Input::~Input()
{
	SDL_free(mGamePadMapping);
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

	UpdateGamePad();
}

bool Input::GetKeyPressDown(int scanCode) const
{
	bool ret = (GetKey(scanCode) && !mPrevStates[scanCode]);
	return ret;
}

bool Input::GetKeyPressUp(int scanCode) const
{
	bool ret = (!GetKey(scanCode) && mPrevStates[scanCode]);
	return ret;
}

void Input::UpdateGamePad()
{
	// 初期化
	mGamePadButtonFlags = 0;

	// ボタン情報を取得
	for (int i = 0; i < SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MAX; ++i)
	{
		Uint16 mask = 0x0001 << i;
		if (SDL_GameControllerGetButton(mGamePad, static_cast<SDL_GameControllerButton>(i)))
		{
			mGamePadButtonFlags |= mask;
		}
	}
}
