#include "Input.h"

Input::Input() :
	mQuitEventFlag(false),
	mGamePadButtonFlags(0),
	mPrevGamePadButtonFlags(0xFFFF)
{
	mStates = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		mPrevStates[i] = 0;
	}

	// ゲームコントローラー取得
	ConnectGamePad(0);

	SDL_GameControllerEventState(SDL_IGNORE);
}

Input::~Input()
{
	DisconnectGamePad(0);
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

		// パッドが接続されたとき
		if (sdlEvent.type == SDL_CONTROLLERDEVICEADDED)
		{
			// ゲーム内で使えるパッドは１つに限る
			if (mGamePad == NULL && sdlEvent.cdevice.which == 0)
			{
				ConnectGamePad(0);
			}
		}

		// パッドが抜かれたとき
		if (sdlEvent.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			// 認証されていないパッドが抜かれてもイベントは呼ばれないようだ
			DisconnectGamePad(0);
		}
	}

	UpdateGamePad();
}

void Input::LastUpdate()
{
	// キーの状態を保存
	SDL_memcpy(mPrevStates, mStates, SDL_NUM_SCANCODES);

	// ボタンの状態を保存
	mPrevGamePadButtonFlags = mGamePadButtonFlags;
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

void Input::ConnectGamePad(int padIndex)
{
	mGamePad = SDL_GameControllerOpen(padIndex);
	if (mGamePad == NULL)
	{
		SDL_Log("Failed to get game controller.\n--%s--\n", SDL_GetError());
	}
	else
	{
		SDL_Log("Success to get game controller.\n");
	}

	mGamePadMapping = SDL_GameControllerMapping(mGamePad);
	if (mGamePadMapping == NULL)
	{
		SDL_Log("Failed to get mapping of game controller.\n--%s--\n", SDL_GetError());
	}
	else
	{
		SDL_Log("Success to get mapping of game controller.\n");
	}
}

void Input::UpdateGamePad()
{
	SDL_GameControllerUpdate();

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

void Input::DisconnectGamePad(int padIndex)
{
	if (mGamePad != NULL)
	{
		SDL_GameControllerClose(mGamePad);
		mGamePad = NULL;
	}

	if (mGamePadMapping != NULL)
	{
		SDL_free(mGamePadMapping);
		mGamePadMapping = NULL;
	}
}
