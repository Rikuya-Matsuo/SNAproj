#include "NinjaArtsUICircle.h"
#include "Texture.h"
#include "Player.h"
#include "Common.h"
#include "System.h"
#include <cmath>

const float NinjaArtsUICircle::mRotateSpeedRadPerSecond = Common::DegToRad(360.0f);

const float NinjaArtsUICircle::mInitialAngle = static_cast<float>(M_PI / 2.0f);

NinjaArtsUICircle::NinjaArtsUICircle(const Vector2D & centerPos, float radius):
	mCenterPosition(centerPos),
	mAngle(mInitialAngle),
	mRadius(radius),
	mIconScale(1.0f),
	mButtonUIScale(1.0f),
	mRRotateFlag(false)
{
	mPrevIconScale = mIconScale;

	mButtonUITexture = System::GetInstance().GetRenderer()->GetTexture("Assets/pad_X.png");
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
	// 角度の更新
	UpdateAngle();

	// アイコンのスケールが変わっており、アイコンが1つ以上登録されていればボタンUIの表示位置を更新する
	if (mPrevIconScale != mIconScale && mTextures.size())
	{
		UpdateButtonUIPosition();
	}

	mPrevIconScale = mIconScale;
}

void NinjaArtsUICircle::UpdateAngle()
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
	// どちらの回転でも同じ場合（アイコンが2つの時）
	// 上の条件文に倣い「絶対値が同じとき」とすると、float型特有のズレのせいで条件文がtrueにならないため、要素数で判断する
	else if (mTextures.size() == 2)
	{
		// 右回転フラグが真なら大きい方、偽なら小さい方を角度差として使う
		angleDiff = mRRotateFlag ?
			Common::Larger(angleDiff, angleDiffMinus) : Common::Smaller(angleDiff, angleDiffMinus);
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

void NinjaArtsUICircle::UpdateButtonUIPosition()
{
	// アイコン画像のサイズ取得
	// 正方形の画像が設定されていることを信じ、x軸の長さを取得する
	int graphSize = mTextures[mCurrentIconID]->GetWidth();

	// ボタンUIの位置を、現在選択されている忍術の上に一旦設定
	GetPositionOf1Texture(mCurrentIconID, mButtonUIPosition);

	// 調節

}
