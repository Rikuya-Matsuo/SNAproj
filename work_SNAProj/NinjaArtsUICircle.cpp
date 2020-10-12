#include "NinjaArtsUICircle.h"
#include "Texture.h"
#include "Player.h"
#include "Common.h"
#include "System.h"
#include <cmath>

const float NinjaArtsUICircle::mRotateSpeedRadPerSecond = Common::DegToRad(240.0f);

const float NinjaArtsUICircle::mInitialAngle = static_cast<float>(M_PI / 2.0f);

NinjaArtsUICircle::NinjaArtsUICircle(const Vector2D & centerPos, float radius):
	mCenterPosition(centerPos),
	mAngle(mInitialAngle),
	mRadius(radius),
	mIconScale(1.0f)
{
}

NinjaArtsUICircle::~NinjaArtsUICircle()
{
	std::vector<Texture *>().swap(mTextures);
}

char NinjaArtsUICircle::ResisterTexture(Texture * tex)
{
	char index = static_cast<char>(mTextures.size());

	mTextures.emplace_back(tex);

	return index;
}

bool NinjaArtsUICircle::GetPositionOf1Texture(size_t index, Vector2D & ret)
{
	if (index >= mTextures.size())
	{
		return false;
	}

	Vector2D offset;
	float angle = mAngle + CalculatDistanceAngle() * index;
	offset.x = cosf(angle);
	offset.y = sinf(angle);
	offset *= mRadius;

	ret = mCenterPosition + offset;

	return true;
}

void NinjaArtsUICircle::Update()
{
	// 目標とする角度
	float goalAngle = CalculatDistanceAngle() * mCurrentIconID + mInitialAngle;

	// この時点で現在の角度と同じなら関数を抜ける
	if (goalAngle == mAngle)
	{
		return;
	}

	// 現在の角度と比較
	float angleDiff = goalAngle - mAngle;

	// 逆回転のほうが早く目標角度に到達できる場合、逆回転をする
	// 目標角度と現在の角度の差をマイナス表記したもの。（例：270度 -> -90度、-30度 -> 330度）
	float piTimes2 = static_cast<float>(2 * M_PI);
	float angleDiffMinus = angleDiff - ((angleDiff > 0) ? piTimes2 : -piTimes2);
	if (fabsf(angleDiff) > fabsf(angleDiffMinus))
	{
		angleDiff = angleDiffMinus;
	}

	// 回転速度の計算
	float speed = mRotateSpeedRadPerSecond * System::GetInstance().GetDeltaTime();
	if (angleDiff < 0.0f)
	{
		speed *= -1;
	}

	// このフレームで目標角度を超えそうならば、mAngleにそのまま代入する
	if (fabsf(angleDiff) < fabsf(speed))
	{
		mAngle = goalAngle;
		return;
	}

	// 回転
	mAngle += speed;
}
