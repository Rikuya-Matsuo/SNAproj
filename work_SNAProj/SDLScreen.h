#pragma once
#include "SDL/SDL.h"

// ���[�h��ʂ�OpenGL�̋@�\���g�킸�ɕ`�悷�邽�߂ɍ��B
// ���[�h��ʈȊO�œ��l�̎d�g�݂𗬗p����\��͂Ȃ����A�O�̂��ߐe�N���X������Ă����B
// ���C���X���b�h�ł͎g���Ȃ��B�ʃX���b�h��Ŏg�p���邱�ƁB
class SDLScreen
{
public:
	SDLScreen();
	virtual ~SDLScreen();

	virtual void Update();

	virtual void Draw(SDL_Renderer * renderer);
};
