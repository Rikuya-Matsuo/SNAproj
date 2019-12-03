#include "SpriteTest.h"
#include "SpriteComponent.h"

SpriteTest::SpriteTest()
{
	SpriteComponent * sc = new SpriteComponent(this);
	sc->LoadTexture("Assets/Enemy01.png");
}

SpriteTest::~SpriteTest()
{
}
