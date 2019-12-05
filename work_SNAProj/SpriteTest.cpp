#include "SpriteTest.h"
#include "SpriteComponent.h"
#include "InputMoveComponent.h"

SpriteTest::SpriteTest():
	Actor()
{
	SpriteComponent * sc = new SpriteComponent(this);
	sc->LoadSprite("Assets/Enemy01.png");

	InputMoveComponent * imc = new InputMoveComponent(this);
}

SpriteTest::~SpriteTest()
{
}

void SpriteTest::Update()
{
	UpdateActor();

	UpdateComponents();
}
