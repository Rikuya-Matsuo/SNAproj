#include "NinjaArtsUICircle.h"
#include "Texture.h"
#include "Player.h"
#include "Common.h"
#include "System.h"
#include <cmath>

const float NinjaArtsUICircle::mRotateSpeedRadPerSecond = Common::DegToRad(360.0f);

const float NinjaArtsUICircle::mInitialAngle = static_cast<float>(M_PI / 4.0f);

NinjaArtsUICircle::NinjaArtsUICircle(const Vector2D & centerPos, float radius):
	mCenterPosition(centerPos),
	mAngle(mInitialAngle),
	mRadius(radius),
	mIconScale(1.0f),
	mButtonUIScale(1.0f),
	mRRotateFlag(false)
{
	// 前フレームでのアイコンのスケール値を初期化
	mPrevIconScale = mIconScale;

	// ボタンUIのテクスチャ
	mButtonUITexture_X = System::GetInstance().GetRenderer()->GetTexture("Assets/pad_X.png");
	mButtonUITexture_Y = System::GetInstance().GetRenderer()->GetTexture("Assets/pad_Y.png");
}

NinjaArtsUICircle::~NinjaArtsUICircle()
{
	// コンテナのクリア
	std::vector<Texture *>().swap(mTextures);
}

char NinjaArtsUICircle::ResisterTexture(Texture * tex)
{
	// 挿入後の添え字となる数値
	char index = static_cast<char>(mTextures.size());

	// テクスチャを挿入
	mTextures.emplace_back(tex);

	return index;
}

bool NinjaArtsUICircle::GetPositionOf1Texture(size_t index, Vector2D & ret)
{
	// indexが登録されているテクスチャの数以上の数値なら取得失敗として関数終了
	if (index >= mTextures.size())
	{
		return false;
	}

	// 指定テクスチャの位置を計算
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

	// スケール値記録
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
	// アイコン画像のサイズ取得 & スケール値を加味
	// 正方形の画像が設定されていることを信じ、x軸の長さを取得する
	float graphSize = mTextures[mCurrentIconID]->GetWidth() * mIconScale;

	// ボタン表示位置設定(Yボタン)
	// ボタンUIの位置を、0番の忍術の上に一旦設定
	GetPositionOf1Texture(0, mButtonUIPosition_Y);

	// 調節
	mButtonUIPosition_Y.x -= graphSize / 2.0f;
	mButtonUIPosition_Y.y += graphSize / 2.0f;

	// ボタン表示位置設定(Xボタン)
	// ボタンUIの位置を、1番の忍術の上に一旦設定
	GetPositionOf1Texture(1, mButtonUIPosition_X);

	// 調節
	mButtonUIPosition_X.x -= graphSize / 2.0f;
	mButtonUIPosition_X.y += graphSize / 2.0f;
}
