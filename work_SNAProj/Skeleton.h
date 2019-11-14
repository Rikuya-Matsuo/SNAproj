#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>
/*
class Skeleton
{
public:
	// Definition for each bone in the skeleton                           それぞれのボーンの骨格定義
	struct Bone
	{
		BoneTransform mLocalBindPose;                                     // ローカルバインドポーズ
		std::string mName;                                                // ボーン名
		int mParent;                                                      // 親ID
	};

	// Load from a file                                                   // ファイルからのロード
	bool Load(const std::string& fileName);

	// Getter functions                                                   // ゲッター
	size_t GetNumBones() const { return mBones.size(); }                  // ボーン数の取得
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }         // idによるボーンの取得
	const std::vector<Bone>& GetBones() const { return mBones; }          // ボーン配列の取得
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; } // グローバル逆バインド行列配列の取得
protected:
	// Called automatically when the skeleton is loaded                     スケルトンがロードされたときに自動的に呼び出されます。       
	// Computes the global inverse bind pose for each bone                  各ボーンのグローバル逆バインドポーズを計算します       
	void ComputeGlobalInvBindPose();
private:
	// The bones in the skeleton                                            ボーン配列
	std::vector<Bone> mBones;
	// The global inverse bind poses for each bone                          それぞれのボーンの逆バインドポーズ行列
	std::vector<Matrix4> mGlobalInvBindPoses;
};
*/