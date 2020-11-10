#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "Matrix.h"
#include "MatrixPalette.h"

// スケルトンの読み込み
bool Skeleton::Load(const std::string& fileName)
{
	// ファイル名からテキストファイルをオープンして、RapidJSONに解析させる
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Skeleton %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	// JSONオブジェクトか
	if (!doc.IsObject())
	{
		SDL_Log("Skeleton %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// メタデータのチェック（バージョンチェック）
	if (ver != 1)
	{
		SDL_Log("Skeleton %s unknown format", fileName.c_str());
		return false;
	}

	// ボーン数の取得
	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
		return false;
	}

	size_t count = bonecount.GetUint();

	// ボーン数がMAX_SKELETON_BONESを超えていた場合 (196本が最大)
	if (count > MAX_SKELETON_BONES)
	{
		SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	mBones.reserve(count);

	// ボーン配列の取得
	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		SDL_Log("Skeleton %s doesn't have a bone array?", fileName.c_str());
		return false;
	}

	// ボーン配列のサイズとボーン数が異なる場合はエラーを返す
	if (bones.Size() != count)
	{
		SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
		return false;
	}

	Bone temp;

	// ボーン配列数分ループ
	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		// ボーンが読めるか？
		if (!bones[i].IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		// name : ボーン名
		const rapidjson::Value& name = bones[i]["name"];
		temp.mName = name.GetString();

		// parent : 親ボーンID
		const rapidjson::Value& parent = bones[i]["parent"];
		temp.mParent = parent.GetInt();

		// bindpose : バインドポーズ
		const rapidjson::Value& bindpose = bones[i]["bindpose"];
		if (!bindpose.IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		// バインドポーズの回転、位置
		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		// 回転と位置が配列じゃなかったらエラー返す
		if (!rot.IsArray() || !trans.IsArray())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		// バインドポーズの回転成分
		temp.mLocalBindPose.mRotation.x = static_cast<float>(rot[0].GetDouble());
		temp.mLocalBindPose.mRotation.y = static_cast<float>(rot[1].GetDouble());
		temp.mLocalBindPose.mRotation.z = static_cast<float>(rot[2].GetDouble());
		temp.mLocalBindPose.mRotation.w = static_cast<float>(rot[3].GetDouble());

		// バインドポーズの移動成分
		temp.mLocalBindPose.mTranslation.x = static_cast<float>(trans[0].GetDouble());
		temp.mLocalBindPose.mTranslation.y = static_cast<float>(trans[1].GetDouble());
		temp.mLocalBindPose.mTranslation.z = static_cast<float>(trans[2].GetDouble());

		// ボーン配列に格納する
		mBones.emplace_back(temp);
	}

	// Now that we have the bones　ここでボーン配列は読みこまれているので、逆バインドポーズ行列を計算する
	ComputeGlobalInvBindPose();

	return true;
}

// 逆バインドポーズ行列の計算
void Skeleton::ComputeGlobalInvBindPose()
{
	// mGlobalInvIndPoses配列を、ボーン数分確保＆自動的に単位行列で初期化
	mGlobalInvBindPoses.resize(GetNumBones());

	// ステップ１：それぞれのボーンのグローバルバインドポーズを計算

	// [0]のグローバルバインドポーズは、ローカルバインドポーズのことである
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// 残りの各ボーンのグローバルバインドポーズは、
	// そのローカルポーズに親のグローバルバインドポーズを掛けたものです。
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
	{
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();                     // そのボーンのローカルバインドポーズを行列に変換して locakMatに代入
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent]; // localMat * 自分の親のグローバルバインドポーズ行列
	}

	// ステップ２：逆行列にする
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}
