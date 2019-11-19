#pragma once
#include "SDL/SDL.h"
#include "ComponentBase.h"

class DrawComponentBase : public ComponentBase
{
public:
	DrawComponentBase(Actor * owner, int drawOrder = 100);
	virtual ~DrawComponentBase();

	virtual void Draw() const = 0;

	int GetDrawOrder() const { return mDrawOrder; }

protected:
	// この値が低いものから順に描画する
	int mDrawOrder;

	SDL_Renderer * mRenderer;
};
