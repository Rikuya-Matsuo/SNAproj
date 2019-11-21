#pragma once
#include "Vector.h"
#include "Matrix.h"
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

	// とりあえず形だけでもエラー消すためにゲッターを定義
	const Matrix4& GetWorldTransform() { return mWorldTransform; }

	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	// コンポーネントの登録・登録解除
	void ResisterComponent(const ComponentBase * in_cmp);
	void DeresisterComponent(const ComponentBase * in_cmp);

protected:
	std::list<ComponentBase *> mComponentList;

	Vector3D mPosition;

	// まだ処理に関しては未実装
	Matrix4 mWorldTransform;

	float mScale;

	void UpdateComponents();

	virtual void UpdateActor();

	void CalculateWorldTransform();
};
