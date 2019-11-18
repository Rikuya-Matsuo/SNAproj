#include "DrawComponentBase.h"
#include "System.h"

DrawComponentBase::DrawComponentBase(Actor * owner, int drawOrder) :
	ComponentBase(owner, 500),
	mDrawOrder(drawOrder),
	mRenderer(nullptr)
{
	mRenderer = const_cast<SDL_Renderer*>(System::GetInstance().GetRenderer());

	System::GetInstance().ResisterDrawComponent(this);
}

DrawComponentBase::~DrawComponentBase()
{
	System::GetInstance().DeresisterDrawComponent(this);
}
