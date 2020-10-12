﻿#pragma once
#include "Vector.h"
#include "SDL/SDL.h"
#include <vector>

class Texture;
class Player;

class NinjaArtsUICircle final
{
public:
	NinjaArtsUICircle(const Vector2D & centerPos, float radius);
	~NinjaArtsUICircle();

	// アイコンテクスチャを登録し、indexを返す
	char ResisterTexture(Texture * tex);

	const std::vector<Texture *> & GetTextures() const { return mTextures; }

	// テクスチャの位置を取得する
	// indexがテクスチャコンテナの要素数異常の数値ならばfalseを返す
	bool GetPositionOf1Texture(size_t index, Vector2D & ret);

	const Vector2D & GetCenterPosition() const { return mCenterPosition; }

	void SetPlayer(Player * player) { mLinkedPlayer = player; }

	void SetIconID(char id) { mCurrentIconID = id; }

	// 右回転を行うか左回転を行うかをフラグによって指定する。
	// しかし、表示アイコンが2つ以下の時のみ、フラグは機能する
	void SetRotateDirection(bool rRotaFlag) { mRRotateFlag = rRotaFlag; }

	float GetIconScale() const { return mIconScale; }

	void SetIconScale(float scale) { mIconScale = scale; }

	void Update();

private:
	static const float mRotateSpeedRadPerSecond;

	static const float mInitialAngle;

	std::vector<Texture *> mTextures;

	const Player * mLinkedPlayer;

	Vector2D mCenterPosition;

	float mRadius;

	float mAngle;

	char mCurrentIconID;

	float mIconScale;

	bool mRRotateFlag;

	// アイコンが空けている角度の間隔の計算
	float CalculatDistanceAngle() { return (2 * static_cast<float>(M_PI) / mTextures.size()); }
};
