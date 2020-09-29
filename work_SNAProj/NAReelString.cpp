#include "NAReelString.h"
#include "BoxColliderComponent.h"
#include "Player.h"
#include "ReelStringEdgeActor.h"

NAReelString::NAReelString(Player* user):
	NinjaArtsBase(user)
{
	mEdge = new ReelStringEdgeActor(user);
}

NAReelString::~NAReelString()
{
}

void NAReelString::Use()
{
	bool lookRight = mUser->GetLookRightFlag();

	mEdge->Launch(lookRight);
}
