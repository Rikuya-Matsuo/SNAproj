#pragma once
#include "NinjaArtsBase.h"

class ColliderComponentBase;
class ReelStringEdgeActor;

class NAReelString : public NinjaArtsBase
{
public:
	NAReelString(Player * user);
	~NAReelString();

	void Use() override;

private:
	ReelStringEdgeActor * mEdge;
};
