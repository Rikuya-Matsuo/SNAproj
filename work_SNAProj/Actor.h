#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include <list>

class ComponentBase;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void Update();

	void SetPosition(const Vector3D & pos) { mPosition = pos; }
	const Vector3D& GetPosition() const { return mPosition; }

	const Matrix4& GetWorldTransform() { return mWorldTransform; }

	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	// コンポーネントの登録・登録解除
	void ResisterComponent(const ComponentBase * in_cmp);
	void DeresisterComponent(const ComponentBase * in_cmp);

protected:
	std::list<ComponentBase *> mComponentList;

	Vector3D mPosition;

	Matrix4 mWorldTransform;

	Quaternion mRotation;

	float mScale;

	void UpdateComponents();

	virtual void UpdateActor();

	void CalculateWorldTransform();
};
