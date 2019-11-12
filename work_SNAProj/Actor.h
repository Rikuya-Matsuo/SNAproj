#pragma once
#include "Vector.h"
#include <list>

class ComponentBase;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void Update();

	void SetPosition(const Vector2D & pos) { mPosition = pos; }
	const Vector2D& GetPosition() const { return mPosition; }

	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	// コンポーネントの登録・登録解除
	void ResisterComponent(const ComponentBase * in_cmp);
	void DeresisterComponent(const ComponentBase * in_cmp);

protected:
	std::list<ComponentBase *> mComponentList;

	Vector2D mPosition;

	float mScale;
};
