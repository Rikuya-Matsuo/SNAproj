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
	// ���̒l���Ⴂ���̂��珇�ɕ`�悷��
	int mDrawOrder;

	SDL_Renderer * mRenderer;
};

