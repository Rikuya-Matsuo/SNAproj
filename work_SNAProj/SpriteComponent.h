#pragma once
#include "ComponentBase.h"

class Texture;
class VertexArray;

class SpriteComponent : public ComponentBase
{
public:
	SpriteComponent(Actor * owner, const Vector3D& size = Vector3D(1.0f, 1.0f, 1.0f));
	~SpriteComponent();

	void Update() override;

private:
	Texture * mTexture;

	VertexArray * mVertexArray;
};
