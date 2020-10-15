#pragma once
#include "Actor.h"

class NAReelString;
class ReelStringEdgeActor;

// プレイヤーと忍術のエッジの間に糸を描画するためだけのアクター
class ReelStringStringActor : public Actor
{
public:
	ReelStringStringActor(NAReelString * ninjaArts, ReelStringEdgeActor * edge);

private:
	NAReelString * mNinjaArts;
	ReelStringEdgeActor * mEdge;

	void UpdateActor0() override;
};
