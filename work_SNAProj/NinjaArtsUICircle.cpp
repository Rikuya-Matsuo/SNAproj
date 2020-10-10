#include "NinjaArtsUICircle.h"
#include "SDL/SDL.h"
#include <cmath>

NinjaArtsUICircle::NinjaArtsUICircle(const Vector2D & centerPos, float radius):
	mCenterPosition(centerPos),
	mAngle(0.0f),
	mRadius(radius)
{
}

NinjaArtsUICircle::~NinjaArtsUICircle()
{
	std::vector<Texture *>().swap(mTextures);
}

bool NinjaArtsUICircle::GetPositionOf1Texture(size_t index, Vector2D & ret)
{
	if (index >= mTextures.size())
	{
		return false;
	}

	Vector2D offset;
	float angle = mAngle + (2 * static_cast<float>(M_PI) / mTextures.size()) * index;
	offset.x = cosf(angle);
	offset.y = sinf(angle);
	offset *= mRadius;

	ret = mCenterPosition + offset;

	return true;
}
