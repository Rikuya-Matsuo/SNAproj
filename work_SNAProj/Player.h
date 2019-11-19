#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
	Player();
	~Player();

	void Update() override;

private:
	void UpdateActor() override;
};
