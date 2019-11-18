#pragma once
#include "Math.h"
#include "BoneTransform.h"
#include <vector>
#include <string>
/*
class Animation
{
public:
	bool Load(const std::string& fileName);                                          // アニメーション読み込み

	size_t GetNumBones() const { return mNumBones; }                                 // ボーン数の取得
	size_t GetNumFrames() const { return mNumFrames; }                               // フレーム数の取得
	float  GetDuration() const { return mDuration; }                                 // アニメーション期間の取得
	float  GetFrameDuration() const { return mFrameDuration; }                       // フレーム期間の取得

	// Fills the provided vector with the global (current) pose matrices for each   指定された配列を、アニメーションの指定された時間に、
	// bone at the specified time in the animation. It is expected that the time	各ボーンのグローバル（現在の）ポーズマトリックスで埋めます。
	// is >= 0.0f and <= mDuration													時間は > = 0.0fかつ <= 期間であると予想される
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const class Skeleton* inSkeleton, float inTime) const;
private:
	// Number of bones for the animation                         アニメーションのためのボーン数
	size_t mNumBones;
	// Number of frames in the animation                         アニメーションのフレーム数
	size_t mNumFrames;
	// Duration of the animation in seconds                      アニメーションの再生時間
	float mDuration;
	// Duration of each frame in the animation                   アニメーションのフレーム間の時刻
	float mFrameDuration;
	// Transform information for each frame on the track         トラック上の各フレームの変換情報。
	// Each index in the outer vector is a bone, inner vector	 外側のベクトルの各インデックスはボーン、
	// is a frame												 内側のベクトルはフレームです。　　　　　　　mTracks[ボーン][フレーム数]
	std::vector<std::vector<BoneTransform>> mTracks;
};
*/