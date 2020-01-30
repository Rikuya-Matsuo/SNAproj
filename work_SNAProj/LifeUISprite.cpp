#include "LifeUISprite.h"
#include "Mesh.h"
#include "System.h"

LifeUISprite::LifeUISprite():
	SpriteActor(nullptr)
{
	mMesh->LoadDivTexture("Assets/flame_parts1.png", System::GetInstance().GetRenderer(), this, 9, 3, 3, 1024, 1024, 0.05f, 0);
}

LifeUISprite::~LifeUISprite()
{
}
