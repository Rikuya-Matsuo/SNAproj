#pragma once
#include "Texture.h"
#include "Vector.h"
#include <vector>

class NinjaArtsUICircle
{
public:
	NinjaArtsUICircle(const Vector2D & centerPos, float radius);
	~NinjaArtsUICircle();

	void Add(Texture * tex) { mTextures.emplace_back(tex); }

	const std::vector<Texture *> & GetTextures() const { return mTextures; }

	// テクスチャの位置を取得する
	// indexがテクスチャコンテナの要素数異常の数値ならばfalseを返す
	bool GetPositionOf1Texture(size_t index, Vector2D & ret);

private:
	std::vector<Texture *> mTextures;

	Vector2D mCenterPosition;

	float mRadius;

	float mAngle;
};
