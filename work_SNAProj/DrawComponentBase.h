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
	// ‚±‚Ì’l‚ª’á‚¢‚à‚Ì‚©‚ç‡‚É•`‰æ‚·‚é
	int mDrawOrder;

	SDL_Renderer * mRenderer;
};

