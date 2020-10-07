#pragma once
#include "Actor.h"

class NAReelString;
class ReelStringEdgeActor;

// �v���C���[�ƔE�p�̃G�b�W�̊ԂɎ���`�悷�邽�߂����̃A�N�^�[
class ReelStringStringActor : public Actor
{
public:
	ReelStringStringActor(NAReelString * ninjaArts, ReelStringEdgeActor * edge);

private:
	NAReelString * mNinjaArts;
	ReelStringEdgeActor * mEdge;

	void UpdateActor0() override;
};
