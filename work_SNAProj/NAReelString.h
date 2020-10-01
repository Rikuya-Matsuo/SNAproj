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

	// 忍術の終了を教える
	void TellEndNinjaArts();

	// 忍術がまだ発動中であることを教える
	void TellRunningNinjaArts();

private:
	ReelStringEdgeActor * mEdge;
};
