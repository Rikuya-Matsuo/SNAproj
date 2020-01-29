#include "SpriteComponent.h"
#include "VertexArray.h"

SpriteComponent::SpriteComponent(Actor * owner, const Vector3D& size):
	ComponentBase(owner)
{
	int xHalf = static_cast<int>(size.x / 2);
	int yHalf = static_cast<int>(size.y / 2);
	int zHalf = static_cast<int>(size.z / 2);
	/*
	unsigned int ver[] =
	{
		-xHalf, 
	}

	mVertexArray = new VertexArray(, 4, VertexArray::Layout::PosNormTex, , 2);
	*/
}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::Update()
{
}
